#include <stdio.h>

#include <math.h>
#include <assert.h>

#include "Util.h"
#include "ArgUtil.h"
#include "StringUtil.h"
#include "Trace.h"

#include "Hash.h"
#include "Hashes.h"

/* Global variables */

Hash pageTable1;
Hash pageTable2;

Frame* frames;
int numberOfFrames;

Algorithm policy;

int pageFaults = 0;
int diskWrites = 0;

/* - - - - - - - - - */

void PageEntry_Init(PageEntry* entry, bool dirty, bool recentlyRead){
    entry->dirty = dirty;
    entry->recentlyRead = recentlyRead;
    // entry->frameNumber is set later on.
}

void CreateMemory(){
    frames = malloc(numberOfFrames * sizeof(Frame));
}

void ParseArgs(int argc, char* argv[], char** algorithm, int* qReferences, int* maxReferences){
    IF_ERROR_MSG(!FindArgAfterFlag(argv, argc, "-a", algorithm), "Argument -a is missing or has no value")

    char* framesStr;
    IF_ERROR_MSG(!FindArgAfterFlag(argv, argc, "-f", &framesStr), "Argument -f is missing or has no value")
    IF_ERROR_MSG(!StringToInt(framesStr, &numberOfFrames, 10), "number of frames should be a positive integer")
    IF_ERROR_MSG(numberOfFrames <= 0,"-f should be > 0")

    char* qReferencesStr;
    IF_ERROR_MSG(!FindArgAfterFlag(argv, argc, "-qr", &qReferencesStr), "Argument -qr is missing or has no value")
    IF_ERROR_MSG(!StringToInt(qReferencesStr, qReferences, 10), "number of q references should be a positive integer")
    IF_ERROR_MSG(*qReferences <= 0,"-qr should be > 0")

    char* maxReferencesStr;
    IF_ERROR_MSG(!FindArgAfterFlag(argv, argc, "-qm", &maxReferencesStr), "Argument -qm is missing or has no value")
    IF_ERROR_MSG(!StringToInt(maxReferencesStr, maxReferences, 10), "number of max references should be a positive integer")
    IF_ERROR_MSG(*maxReferences <= 0,"-qm should be > 0")
}

unsigned int GetVirtualPageNumber(char* address, int pageSize){
    unsigned int numberOfOffsetBits = (int)log2(pageSize);

    int addressVal;
    StringToInt(address, &addressVal, 16);

    return (unsigned int)addressVal >> numberOfOffsetBits;
}

bool UIntCompare(void* val1, void* val2){
    return *(unsigned int*)val1 ==  *(unsigned int*)val2;
}

void ReplaceFirst(PageEntry* newPage, unsigned int pageNumber, unsigned int pid, List* memoryReplacementHelper){
    PageEntry* oldPage = memoryReplacementHelper->head->value;
    unsigned int oldFrameNumber = oldPage->frameNumber;
    unsigned int oldPID = frames[oldFrameNumber].currentPID;
    unsigned int oldPageNumber = frames[oldFrameNumber].currentPageNumber;

    if(oldPage->dirty)
        diskWrites++;

    // Remove old page from old page table.
    Hash* tempTable = (oldPID == PID1) ? &pageTable1 : &pageTable2;
    Hash_Remove(tempTable, &oldPageNumber, sizeof(oldPageNumber));
    free(oldPage);

    // Update frame info.
    frames[oldFrameNumber].currentPageNumber = pageNumber;
    frames[oldFrameNumber].currentPID = pid;
    newPage->frameNumber = oldFrameNumber;

    // Update LRU stack.
    List_Remove(memoryReplacementHelper, 0);
    List_Append(memoryReplacementHelper, newPage);
}

void LRU_Update(PageEntry* pageRead, List* memoryReplacementHelper){
    Node* currPageNode = memoryReplacementHelper->head;
    while(currPageNode != NULL){
        PageEntry* currPage = currPageNode->value;
        if(currPage == pageRead){
            List_RemoveNode(memoryReplacementHelper, currPageNode);
            List_Append(memoryReplacementHelper, pageRead);
            return;
        }

        currPageNode = currPageNode->next;
    }

    assert(true); // pageRead SHOULD be inside the helper list.
}

void SC_Replace(PageEntry* newPage, unsigned int pageNumber, unsigned int pid, List* memoryReplacementHelper){
    while(true) {
        Node *currPageNode = memoryReplacementHelper->head;
        PageEntry *currPage = currPageNode->value;
        if (currPage->recentlyRead) {
            currPage->recentlyRead = false;
            List_RemoveNode(memoryReplacementHelper, currPageNode);
            List_Append(memoryReplacementHelper, currPage);
        } else {
            ReplaceFirst(newPage, pageNumber, pid, memoryReplacementHelper);
            return;
        }
    }
}

void PageFault(PageEntry* newPage, unsigned int pageNumber, unsigned int pid, List* memoryReplacementHelper){
    static int framesFilled = 0;

    pageFaults++;

    if(framesFilled < numberOfFrames) {
        newPage->frameNumber = framesFilled;
        frames[framesFilled].currentPageNumber = pageNumber;
        frames[framesFilled].currentPID = pid;
        framesFilled++;
        List_Append(memoryReplacementHelper, newPage);
    }else {
        // We didnt find a gap , a replacement method is needed.
        if(policy == LRU){
            ReplaceFirst(newPage, pageNumber, pid, memoryReplacementHelper);
        }else{
            SC_Replace(newPage, pageNumber, pid, memoryReplacementHelper);
        }
    }
}

int main(int argc, char* argv[]) {
    // - - - - Setup - - - -
    char* algorithm;
    int qReferences, maxReferences;
    ParseArgs(argc, argv, &algorithm, &qReferences, &maxReferences);

    policy = (strcmp(algorithm, "LRU") == 0) ? LRU : Second_Chance;

    FILE* traceFile1 = fopen(TRACE_FILE1_PATH, "r");
    FILE* traceFile2 = fopen(TRACE_FILE2_PATH, "r");

    Hash_Init(&pageTable1, DEFAULT_HASH_SIZE, RSHash, UIntCompare, true);
    Hash_Init(&pageTable2, DEFAULT_HASH_SIZE, RSHash, UIntCompare, true);

    CreateMemory();

    List memoryReplacementHelper;
    List_Init(&memoryReplacementHelper);

    FILE* currFile = traceFile1;
    Hash* currPageTable = &pageTable1;

    bool file1EOF = false;
    bool file2EOF = false;

    int referencesCounter = 0;

    // - - - - Main loop - - - -

    /* Keep reading if there is at least a file that hasn't reached EOF
     * and we have not reached the maximum counted references */
    char buffer[BUFFER_SIZE];
    while((!file1EOF || !file2EOF) && referencesCounter < maxReferences) {
        bool isFile1 = currFile == traceFile1;
        // Read q references but stop if we reached the maximum count.
        for (int i = 0; i < qReferences && referencesCounter < maxReferences; ++i) {
            bool isEOF = fgets(buffer, BUFFER_SIZE, currFile) == NULL;
            if(isEOF){
                if(isFile1){
                    file1EOF = true;
                }else{
                    file2EOF = true;
                }

                break;
            }

            // - - - handle reference - - -
            Trace trace;
            Trace_Init(&trace, buffer);

            unsigned int vp = GetVirtualPageNumber(trace.address, PAGE_SIZE);

            PageEntry* pageEntry = Hash_GetValue(*currPageTable, &vp, sizeof(vp));
            if(pageEntry == NULL){
                // the page isn't in memory.
                pageEntry = malloc(sizeof(PageEntry));
                PageEntry_Init(pageEntry, trace.type == Write, true);
                Hash_Add(currPageTable, &vp, sizeof(vp), pageEntry);

                unsigned pid = (isFile1) ? PID1 : PID2;
                PageFault(pageEntry, vp, pid, &memoryReplacementHelper);
            }else {
                // Reference inside already placed memory.
                PageEntry_Init(pageEntry, pageEntry->dirty || trace.type == Write, true);
                if (policy == LRU) {
                    LRU_Update(pageEntry, &memoryReplacementHelper);
                }
                // 2nd chance Update isn't needed we update the reference bit each time anyway.
            }

            Trace_Destroy(&trace);
            referencesCounter++;
            // - - - - - - - - - - - - - - -
        }

        // Swap files if only the other one hasn't read EOF
        if(isFile1){
            if(!file2EOF) {
                currPageTable = &pageTable2;
                currFile = traceFile2;
            }
        }else{
            if(!file1EOF)  {
                currPageTable = &pageTable1;
                currFile = traceFile1;
            }
        }
    }

    // - - - - Write back remaining pages if needed - - - -
    int diskWritesAtTheEnd = 0;
    Node* currPage = memoryReplacementHelper.head;
    while(currPage != NULL){
        PageEntry* currPE = currPage->value;
        if(currPE->dirty) {
            diskWritesAtTheEnd++;
            diskWrites++;
        }

        currPage = currPage->next;
    }

    // - - - - Print stats - - - -

    printf("Page faults : %d\n", pageFaults);
    printf("Disk writes before cleanup : %d\n", diskWrites - diskWritesAtTheEnd);
    printf("Disk writes after cleanup : %d\n", diskWrites);

    // - - - - Clean up - - - -

    free(frames);

    List_Destroy(&memoryReplacementHelper);

    Hash_FreeValues(pageTable1, free);
    Hash_Destroy(pageTable1);
    Hash_FreeValues(pageTable2, free);
    Hash_Destroy(pageTable2);

    fclose(traceFile1);
    fclose(traceFile2);

    return 0;
}
