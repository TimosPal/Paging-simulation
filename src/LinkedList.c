#include "LinkedList.h"

#include <stdio.h>
#include <stdlib.h>

void List_Init(List* list) {
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

void List_Destroy(List* list) {
	while (list->head != NULL){
		List_Remove(list, 0);
	}
}

void List_Append(List* list, void* value) {
	Node* newNode = malloc(sizeof(Node));
	newNode->value = value;
	newNode->next = NULL;

	if(list->head == NULL){
        list->head = newNode;
        newNode->prev = NULL;
    }else{
        list->tail->next = newNode;
        newNode->prev = list->tail;
    }
	list->tail = newNode;
	list->size++;
}

bool List_Remove(List* list, int index) {
	if ( list->head == NULL ){
		return false;
	}
	Node* temp = list->head;
	(list->size)--;

	if (index == 0) {
		list->head = list->head->next;
		free(temp);

		if (list->head == NULL) {
            list->tail = NULL;
        }else{
		    list->head->prev = NULL;
		}
	} else {
		for (int i = 0; i < index - 1; i++)
			temp = temp->next;

		Node* old = temp->next;
		temp->next = old->next;
		free(old);

		if (temp->next == NULL) {
			list->tail = temp;
		}else{
		    temp->next->prev = temp;
		}
	}
	

	return true;
}

bool List_RemoveNode(List* list, Node* node){
    if(node->prev == NULL){ // we remove the first node.
        list->head = node->next;
    }else{
        node->prev->next = node->next;
    }

    if(node->next == NULL){ // we remove the last node.
        list->tail = node->prev;
    }else{
        node->next->prev = node->prev;
    }

    free(node);

	(list->size)--;

	return true;
}

void List_FreeValues(List list, void (*subFree)(void*)){
	Node* currNode = list.head;
	while(currNode != NULL){
		subFree(currNode->value);

		currNode = currNode->next;
	}
}