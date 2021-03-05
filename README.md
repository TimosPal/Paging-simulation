# Paging-simulation
We simulate a paging system. Two processes read two trace files in an alternating pattern. Each processes has its own page table
(represented as a hash). Two swapping policies can be tested {LRU / Second chance}. The trace files contain addresses and the character
R if it's a reading request , or W if it's a writing request respectively.

Note : this was a university assignment.

## How to compile:
- make

## Ho to run:
- ./prog <args>
- The trace files should be in the parent directory (parametric files not supported).

## Arguments:

- a : algorithm LRU / SC
- f : number of frames
- qr : q references per iteration
- am : maximum number of references to be read in the simulation

Example:

./prog -a LRU -f 1000 -qr 80 -qm 13900

## Implementation details:

A double linkedlist and a hashtable are used.
The hashtable is used for the Page-table representation whereas the linked list
for the LRU / SC "stack/queue"

The trace struct represent the currently read trace , having its type {read/rite} and the address it is
trying to access.

The frame struct represents a page in memory. It has 2 member variables , currentPageNumber 
and currentPID. We require the PID of each active page so we know which page table to alter if a page fault
occurs in said frame's page. (We remove page table entries that are not active in memory , so when a new reference
comes we only need to see if the entry is contained in the page table , if not a page fault occurs.)

Finaly a page entry struct represents the info a newly added page contains. It contains its frameNumber (index) in a corresponding array of frames if it a write has occursed (the page is dirty) and if it has been recentlyRead (used by SC)

## Results:

We print the number of Page faults and Disk writes before and after the final cleanup. (when the simulation ends there are still pages in memory). We dont print Disk reads since a page fault is followed by a page read so we know
page reads == page faults.
