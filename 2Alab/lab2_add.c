//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "utils.h"

struct arg_struct {
	int iterations;
	long long* pointer;
};

int opt_yield = 0;
void add(long long* pointer, long long value)
{
	long long sum = *pointer + value;
	if(opt_yield) 
		sched_yield();
	*pointer = sum;
}

void* pthreader(void* arguments)
{
	struct arg_struct *args = arguments;
	int iterations = args->iterations;
	long long* pointer = args->pointer;
	//fprintf(stderr, "RM: %d iterations. %lld sum\n", iterations, *pointer);
	
	int i;
	for(i = 0; i < iterations; i++)
	{
		add(pointer, 1);
	}
	// fprintf(stderr, "RM: mid: %d iterations. %lld sum\n", iterations, *pointer);
	for(i = 0; i < iterations; i++)
	{
		add(pointer, -1);
	}
	// fprintf(stderr, "RM: final: %d iterations. %lld sum\n", iterations, *pointer);

	pthread_exit(NULL);
	return NULL;
}

int main(int argc, char* argv[])
{
	struct timespec timer1;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1) != 0)
	{
		fprintf(stderr, "failed to get time 1\n");
		exit (2);
	}
	//long long counter = 0;

	int c;
	int option_index = 0;
	static struct option long_options[] =
	{
		{"threads", 	required_argument, 	0, 't'},
		{"iterations",	required_argument, 	0, 'i'},
		{"yield", 	no_argument,		0, 'y'},
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
				opt_yield = 1;
				break;
			case '?':
				fprintf(stderr, "invaild argument\n");
				exit(1);
				break;
			default:
				fprintf(stderr, "FATAL\n"); //never supposed to happend
		}
	}

	//running the threads now
	pthread_t threadids[num_threads];
	long long sum = 0;
	struct arg_struct args;
	args.iterations = num_iterations;
	args.pointer = &sum;
	
	int i;
	for(i = 0; i < num_threads; i++)
	{
		if(pthread_create(&threadids[i], NULL, &pthreader, (void*)&args) != 0)
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

	//final printing
	int tot_it = num_threads * num_iterations * 2;
	char* name = "add-none";
	if(opt_yield)
	{
		name = "add-yield";
	}
	else
	{
		name = "add-yield-none";
	}

	struct timespec timer2;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2) != 0)
	{
		fprintf(stderr, "failed to get time 2\n");
		exit (2);
	}
	//if(timer2.tv_sec - timer1.tv_sec > 0) fprintf(stderr, "hmmm...\n");
	int f_time = timer2.tv_nsec - timer1.tv_nsec;
	int avg_time = f_time / tot_it;

	fprintf(stderr, "%s,%d,%d,%d,%d,%d,%lld\n", name, num_threads, num_iterations,
		tot_it, f_time, avg_time, sum);
	fprintf(stdout, "%s,%d,%d,%d,%d,%d,%lld\n", name, num_threads, num_iterations,
		tot_it, f_time, avg_time, sum);
	return 0;
}
