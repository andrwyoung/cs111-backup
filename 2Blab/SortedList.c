//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include "SortedList.h"
#include <stdio.h>
#include <string.h>
#include <sched.h>

void SortedList_insert(SortedList_t* list, SortedListElement_t* element)
{
	//travel all the way around
	struct SortedListElement* mark = list;
	for(; mark->next != list; mark = mark->next) 
	{
		if(strcmp(mark->next->key, element->key) > 0)
			break;
	}
	//fprintf(stderr, "RM: inserting %s\n", element->key);
	
	mark->next->prev = element;
	element->next = mark->next;
	if(opt_yield & INSERT_YIELD) 
		sched_yield();
	element->prev = mark;
	mark->next = element;
}

int SortedList_delete(SortedListElement_t* element)
{
	if(element->next->prev != element || element->prev->next != element)
	{
		fprintf(stderr, "corrupted list\n");
		return -1;
	}

	element->next->prev = element->prev;
	element->prev->next = element->next;
	if(opt_yield & DELETE_YIELD)
		sched_yield();
	element->prev = NULL;
	element->next = NULL;
	return 0;
}

SortedListElement_t* SortedList_lookup(SortedList_t* list, const char* key)
{
	struct SortedListElement* mark = list->next;
	for(; mark->next != list; mark = mark->next)
	{
		//fprintf(stderr, "RM in lookup: %s\n", mark->key);
		if(mark->next == NULL)
		{
			fprintf(stderr, "corrupted list\n");
			return NULL;
		}
		
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		if(strcmp(mark->key, key) == 0) return mark;
	}
	//fprintf(stderr, "RM lookup: %s\n", mark->key);
	if(strcmp(mark->key, key) == 0) return mark;

	//if for loop end, it's not found
	fprintf(stderr, "corrupted list\n");
	return NULL;
}

int SortedList_length(SortedList_t* list)
{
	int count = 0;
	struct SortedListElement* mark = list;
	for(; mark->next != list; mark = mark->next) 
	{
		if(mark->next == NULL)
		{
			fprintf(stderr, "corrupted list\n");
			return -1;
		}
		
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		count++;
	}
	return count;
}
