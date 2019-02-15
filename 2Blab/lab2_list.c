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

int breakpoints = 0;

int sync_op = 0;
int opt_yield = 0;
struct arg_struct {
	int num_lists;
	int iterations;
	long long wait_time;
	SortedListElement_t* elements;
	struct list_struct* lists;
};

struct list_struct {
	SortedList_t* head;
	pthread_mutex_t mutexd;
	volatile int spind;
};

void here(int hmm)
{
	fprintf(stderr, "hmm: %d\n", hmm);
}

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

int hasher(const char* string)
{
	int counter = 0;
	unsigned long i;
	for(i = 0; i < strlen(string); i++)
	{
		counter += string[i];
	}
	return counter;
}

void* pthreader(void* arguments)
{
	struct timespec timer1;
	struct timespec timer2;
	
	//fill out the struct
	struct arg_struct* args = arguments;
	SortedListElement_t* elements = args->elements;
	int iterations = args->iterations;
	int num_lists = args->num_lists;
	
	if(breakpoints) fprintf(stderr, "DEBUG: declared pthreader variables\n");

	int i;
	for(i = 0; i < iterations; i++)
	{
		//which head to insert into
		int head_num = hasher(elements[i].key) % num_lists;
		SortedList_t* head = args->lists[head_num].head;
		pthread_mutex_t* mutexd = &args->lists[head_num].mutexd;
		volatile int* spind = &args->lists[head_num].spind;
		//fprintf(stderr, "RM: insert hashed num: %d for key %s\n", head_num, elements[i].key);
		
		//insert
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1);
		if(sync_op == 1) pthread_mutex_lock(mutexd);
		if(sync_op == 2) while(__sync_lock_test_and_set(spind, 1)) continue;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2);
		args->wait_time += timer2.tv_nsec - timer1.tv_nsec + 
			((timer2.tv_sec - timer1.tv_sec) * 1000000000);

		SortedList_insert(head, &elements[i]); //here!
		if(sync_op == 1) pthread_mutex_unlock(mutexd);
		if(sync_op == 2) __sync_lock_release(spind);
	}

	if(breakpoints) fprintf(stderr, "DEBUG: got through inserting\n");

	//length 
	for(i = 0; i < num_lists; i++)
	{
		pthread_mutex_t* mutexd = &args->lists[i].mutexd;
		volatile int* spind = &args->lists[i].spind;

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1);
		if(sync_op == 1) pthread_mutex_lock(mutexd);
		if(sync_op == 2) while(__sync_lock_test_and_set(spind, 1)) continue;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2);
		args->wait_time += timer2.tv_nsec - timer1.tv_nsec + 
		((timer2.tv_sec - timer1.tv_sec) * 1000000000);

		//fprintf(stderr, "length for head %d: %d\n", i, SortedList_length(args->lists[i].head));
		if(SortedList_length(args->lists[i].head) == -1) exit(2); //here!
	
		if(sync_op == 1) pthread_mutex_unlock(mutexd);
		if(sync_op == 2) __sync_lock_release(spind);
	}
	

	for(i = 0; i < iterations; i++)
	{
		//which head to go to lookup
		int head_num = hasher(elements[i].key) % num_lists;
		SortedList_t* head = args->lists[head_num].head;
		pthread_mutex_t* mutexd = &args->lists[head_num].mutexd;
		volatile int* spind = &args->lists[head_num].spind;
		//fprintf(stderr, "RM: lookup hashed num: %d for key %s\n", head_num, elements[i].key);
		
	
		//lookup
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1);
		if(sync_op == 1) pthread_mutex_lock(mutexd);
		if(sync_op == 2) while(__sync_lock_test_and_set(spind, 1)) continue;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2);
		args->wait_time += timer2.tv_nsec - timer1.tv_nsec + 
			((timer2.tv_sec - timer1.tv_sec) * 1000000000);

		SortedListElement_t* found = SortedList_lookup(head, elements[i].key); //here!
		if(found == NULL) exit(2); //check
		if(sync_op == 1) pthread_mutex_unlock(mutexd);
		if(sync_op == 2) __sync_lock_release(spind);


		//deleting
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1);
		if(sync_op == 1) pthread_mutex_lock(mutexd);
		if(sync_op == 2) while(__sync_lock_test_and_set(spind, 1)) continue;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2);
		args->wait_time += timer2.tv_nsec - timer1.tv_nsec + 
			((timer2.tv_sec - timer1.tv_sec) * 1000000000);
			
		if(SortedList_delete(found) == -1) exit(2); //here!
		if(sync_op == 1) pthread_mutex_unlock(mutexd);
		if(sync_op == 2) __sync_lock_release(spind);
	}

	if(sync_op == 0) args->wait_time = 0; //we're cheating here
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
		case 0:
			fprintf(stdout, "none");
			break;
		case 1:
			fprintf(stdout, "m");
			break;
		case 2:
			fprintf(stdout, "s");
			break;
		default:
			fprintf(stdout, "FATAL");
			break;
	}
}


int main(int argc, char* argv[])
{
	signal(SIGSEGV, catcher);
	
	//setting up and doing getopt
	int c;
	int option_index = 0;
	static struct option long_options[] = 
	{
		{"threads", 	required_argument,	0, 't'},
		{"iterations",	required_argument,	0, 'i'},
		{"yield", 	required_argument,	0, 'y'},
		{"sync", 	required_argument,	0, 's'},
		{"lists", 	required_argument,	0, 'l'},
		{0, 0, 0, 0}
	};

	int num_threads = 1;
	int num_iterations = 1;
	int num_lists = 1;
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
						sync_op = 1;
						break;
					case 's':
						sync_op = 2;
						break;
					default:
						fprintf(stderr, "invaild sync option\n");
						exit (1);
				} 
				break;
			case 'l':
				{
					int num = stoi(optarg);
					if(num < 0)
					{
						fprintf(stderr, "input for --lists not a number\n");
						exit(1);
					}
					//fprintf(stderr, "RM: %s lists\n", optarg);
					num_lists = num;
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

	int i; //for all my looping
	srand(time(0)); //initialize rand()

	//create all the heads of the lists and mutexes
	struct list_struct lists[num_lists];
	for(i = 0; i < num_lists; i++)
	{	
		//create a new head
		SortedList_t* head = malloc(sizeof(SortedList_t));
		head->key = NULL;
		head->prev = head;
		head->next = head;
		
		lists[i].head = head;
		if(sync_op == 1) pthread_mutex_init(&lists[i].mutexd, NULL);
		if(sync_op == 2) lists[i].spind = 0;

	}	
	
	//create all those keys and elements
	int tot = num_threads * num_iterations;
	SortedListElement_t elements[tot];
	for(i = 0; i < tot; i++)
	{
		SortedListElement_t temp;
		temp.key = rand_str(KEY_LEN);
		elements[i] = temp;
	}

	
	//initalizing the timer
	struct timespec timer1;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1) != 0)
	{
		fprintf(stderr, "failed to get time 1\n");
		exit (2);
	}



	if(breakpoints) fprintf(stderr, "DEBUG: before the threads\n");
	//running the threads now
	pthread_t threadids[num_threads];
	struct arg_struct* args[num_threads];
	for(i = 0; i < num_threads; i++)
	{
		//setting up struct
		args[i] = malloc(sizeof(struct arg_struct));
		args[i]->iterations = num_iterations;
		args[i]->elements = elements + (i * num_iterations);
		args[i]->lists = lists;
		args[i]->num_lists = num_lists;
		args[i]->wait_time = 0;
		//fprintf(stderr, "RM: putting in %s\n", args[i]->elements[0].key);
		if(pthread_create(&threadids[i], NULL, &pthreader, args[i]) != 0)
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
	if(breakpoints) fprintf(stderr, "DEBUG: right after joining the threads\n");

	//ending the timer
	struct timespec timer2;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2) != 0)
	{
		fprintf(stderr, "failed to get time 2\n");
		exit (2);
	}

	//checking to see that it went back to 0
	//destroy mutex while you at it
	for(i = 0; i < num_lists; i++)
	{
		if(SortedList_length(lists[i].head) != 0)
		{
			fprintf(stderr, "Error: length not back to 0\n");
		}
		if(sync_op == 1) pthread_mutex_destroy(&lists[i].mutexd);
		free((void*)lists[i].head);
	}
	//getting all the data to print. free stuff while you at it
	long long wait_time = 0;
	for(i = 0; i < tot; i++)
	{
		free((char*)elements[i].key);
	}
	for(i = 0; i < num_threads; i++)
	{
		//get the wait times for each thread
		wait_time += args[i]->wait_time;
		free(args[i]);
	}

	//get the rest of the data to print
	int tot_op = num_threads * num_iterations * 3; 
	long long f_time = timer2.tv_nsec - timer1.tv_nsec + 
		((timer2.tv_sec - timer1.tv_sec) * 1000000000);
	long long avg_time = f_time / tot_op;
	//now actually print if
	namer(); //prints out the first item
	fprintf(stdout, ",%d,%d,%d,%d,%lld,%lld,%lld\n", num_threads, num_iterations,
		num_lists, tot_op, f_time, avg_time, wait_time);

	return 0;
}
