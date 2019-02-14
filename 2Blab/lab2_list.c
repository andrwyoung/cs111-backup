//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <ctype.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "SortedList.h"
#define KEY_LEN 10

pthread_mutex_t mutexd;
volatile int spind = 0;
char sync_op = 'n';

typedef void* (*pthreader_type)(void* arguments);

int opt_yield = 0;
struct arg_struct {
	int iterations;
	SortedListElement_t* elements;
	SortedList_t* head;
};

void catcher()
{
	fprintf(stderr, "segmentation fault\n");
	exit(2);
}

int stoi(char* string)
{
	unsigned int i;
	for(i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
		{
			fprintf(stderr, "not a number\n");
			return -1;
		}
	}
	return atoi(string);
}

char* rand_str(int length)
{
	char* temp = malloc(sizeof(char) * length);
	char charset[] = "0123456789"
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";		
	int i;
	char rand_char;
	for(i = 0; i < length; i++)
	{	
		rand_char = charset[rand() % 62];
		temp[i] = rand_char;
	}
	return temp;
}


void* pthreader(void* arguments)
{
	struct arg_struct* args = arguments;
	SortedListElement_t* list = args->elements;

	int i;
	for(i = 0; i < args->iterations; i++)
	{
		SortedList_insert(args->head, &list[i]);
	}

	
	if(SortedList_length(args->head) == -1) exit(2);

	for(i = 0; i < args->iterations; i++)
	{
		SortedListElement_t* found = SortedList_lookup(args->head, list[i].key);
		if(found == NULL) exit(2);
		if(SortedList_delete(found) == -1) exit(2);
	}

	pthread_exit(NULL);
}

void* mutex_pthreader(void* arguments)
{
	struct arg_struct* args = arguments;
	SortedListElement_t* list = args->elements;

	int i;
	for(i = 0; i < args->iterations; i++)
	{
		pthread_mutex_lock(&mutexd);
		SortedList_insert(args->head, &list[i]);
		pthread_mutex_unlock(&mutexd);
	}

	
	pthread_mutex_lock(&mutexd);	
	if(SortedList_length(args->head) == -1) exit(2);
	pthread_mutex_unlock(&mutexd);


	for(i = 0; i < args->iterations; i++)
	{
		pthread_mutex_lock(&mutexd);
		SortedListElement_t* found = SortedList_lookup(args->head, list[i].key);
		if(found == NULL) exit(2);
		pthread_mutex_unlock(&mutexd);

		pthread_mutex_lock(&mutexd);
		if(SortedList_delete(found) == -1) exit(2);
		pthread_mutex_unlock(&mutexd);
	}

	pthread_exit(NULL);
}

void* spin_pthreader(void* arguments)
{
	struct arg_struct* args = arguments;
	SortedListElement_t* list = args->elements;

	int i;
	for(i = 0; i < args->iterations; i++)
	{
		while(__sync_lock_test_and_set(&spind, 1)) continue;
		SortedList_insert(args->head, &list[i]);
		__sync_lock_release(&spind);
	}

	
	while(__sync_lock_test_and_set(&spind, 1)) continue;
	if(SortedList_length(args->head) == -1) exit(2);
	__sync_lock_release(&spind);


	for(i = 0; i < args->iterations; i++)
	{
		while(__sync_lock_test_and_set(&spind, 1)) continue;
		SortedListElement_t* found = SortedList_lookup(args->head, list[i].key);
		if(found == NULL) exit(2);
		__sync_lock_release(&spind);
		
		while(__sync_lock_test_and_set(&spind, 1)) continue;
		if(SortedList_delete(found) == -1) exit(2);
		__sync_lock_release(&spind);
	}

	pthread_exit(NULL);
}

void namer()
{
	fprintf(stdout, "list-");
	if(opt_yield & INSERT_YIELD) fprintf(stdout, "i");
	if(opt_yield & DELETE_YIELD) fprintf(stdout, "d");
	if(opt_yield & LOOKUP_YIELD) fprintf(stdout, "l");
	if(opt_yield == 0) fprintf(stdout, "none");
	fprintf(stdout, "-");
	
	switch(sync_op)
	{
		case 's':
			fprintf(stdout, "s");
			break;
		case 'm':
			fprintf(stdout, "m");
			break;
		case 'n':
			fprintf(stdout, "none");
			break;
		default:
			fprintf(stdout, "FATAL");
			break;
	}
}


int main(int argc, char* argv[])
{
	pthreader_type func = pthreader;
	signal(SIGSEGV, catcher);
	
	//setting up and doing getopt
	char c;
	int option_index = 0;
	static struct option long_options[] = 
	{
		{"threads", 	required_argument,	0, 't'},
		{"iterations",	required_argument,	0, 'i'},
		{"yield", 	required_argument,	0, 'y'},
		{"sync", 	required_argument,	0, 's'},
		{0, 0, 0, 0}
	};

	int num_threads = 1;
	int num_iterations = 1;
	while(1)
	{
		c = getopt_long(argc, argv, "", long_options, &option_index);
		if(c == -1)
			break;

		switch(c)
		{
			case 't':
				{
					int num = stoi(optarg);
					if(num < 0)
					{
						fprintf(stderr, "input for --threads not a number\n");
						exit(1);
					}
					//fprintf(stderr, "RM: %s threads\n", optarg);
					num_threads = num;
				}
				break;
			case 'i':
				{
					int num = stoi(optarg);
					if(num < 0)
					{
						fprintf(stderr, "input for --threads not a number\n");
						exit(1);
					}
					//fprintf(stderr, "RM: %s iterations\n", optarg);
					num_iterations = num;
				}
				break;
			case 'y':
				{
					unsigned long i;
					for(i = 0; i < strlen(optarg); i++)
					{
						switch(optarg[i])
						{
							case 'i':
								opt_yield |= INSERT_YIELD;
								break;
							case 'd':
								opt_yield |= DELETE_YIELD;
								break;
							case 'l':
								opt_yield |= LOOKUP_YIELD;
								break;
							default:
								fprintf(stderr, "invalid yield option\n");
								exit (1);
						}
					}
				}
				break;
			case 's':
				switch(optarg[0])
				{
					case 'm':
						pthread_mutex_init(&mutexd, NULL);
						func = mutex_pthreader;
						sync_op = 'm';
						break;
					case 's':
						func = spin_pthreader;
						sync_op = 's';
						break;
					default:
						fprintf(stderr, "invaild sync option\n");
						exit (1);
				} 
				break;
			case '?':
				fprintf(stderr, "invaild argument\n");
				exit(1);
				break;
			default:
				fprintf(stderr, "FATAL\n"); //never supposed to happend
		}
	}

	SortedList_t head;
	head.key = NULL;
	head.prev = &head;
	head.next = &head;
	srand(time(0)); //initialize rand()

	//create all those sorted lists
	int i; //for all my looping
	int tot = num_threads * num_iterations;
	SortedListElement_t list[tot];
	for(i = 0; i < tot; i++)
	{
		SortedListElement_t temp;
		temp.key = rand_str(KEY_LEN);
		//fprintf(stderr, "RM: key: %s index: %d\n", temp.key, i);
		list[i] = temp;
	}

	
	//initalizing the timer
	struct timespec timer1;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1) != 0)
	{
		fprintf(stderr, "failed to get time 1\n");
		exit (2);
	}



	
	//running the threads now
	pthread_t threadids[num_threads];
	struct arg_struct* args[num_threads];
	for(i = 0; i < num_threads; i++)
	{
		//setting up struct
		args[i] = malloc(sizeof(struct arg_struct));
		args[i]->iterations = num_iterations;
		args[i]->elements = list + (i * num_iterations);
		args[i]->head = &head;
		//fprintf(stderr, "RM: putting in %s\n", args[i]->elements[0].key);
		if(pthread_create(&threadids[i], NULL, func, args[i]) != 0)
		{
			fprintf(stderr, "failed to create thread\n");
			exit (2);
		}
	}

	
	//waiting for threads
	for(i = 0; i < num_threads; i++)
	{
		if(pthread_join(threadids[i], NULL) != 0)
		{
			fprintf(stderr, "failed to join thread\n");
			exit (2);
		}
	}	

	//checking to see that it went back to 0
	if(SortedList_length(&head) != 0)
	{
		fprintf(stderr, "Error: length not back to 0\n");
	}


	//ending the timer
	struct timespec timer2;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2) != 0)
	{
		fprintf(stderr, "failed to get time 2\n");
		exit (2);
	}


	//getting all the data to print
	int tot_lists = 1;
	int tot_op = num_threads * num_iterations * 3;
	int f_time = timer2.tv_nsec - timer1.tv_nsec;
	int avg_time = f_time / tot_op;
	
	namer();
	fprintf(stdout, ",%d,%d,%d,%d,%d,%d\n", num_threads, num_iterations,
		tot_lists, tot_op, f_time, avg_time);


	//free everything
	for(i = 0; i < tot; i++)
	{
		free((char*)list[i].key);
	}
	for(i = 0; i < num_threads; i++)
	{
		free(args[i]);
	}
	pthread_mutex_destroy(&mutexd);
	return 0;
}
