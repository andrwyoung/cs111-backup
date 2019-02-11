//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <ctype.h>


pthread_mutex_t mutexd;
volatile int spind = 0;

int opt_yield = 0;

typedef void (*add_func)(long long* pointer, long long value);
struct arg_struct {
	int iterations;
	long long* pointer;
	add_func func; //which function to use
};

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


void add(long long* pointer, long long value)
{
		//fprintf(stderr, "RM: regular add\n");
	long long sum = *pointer + value;
	if(opt_yield) 
		sched_yield();
	*pointer = sum;
}

void mut_add(long long* pointer, long long value)
{
		//fprintf(stderr, "RM: mut add\n");
	pthread_mutex_lock(&mutexd);
	
	long long sum = *pointer + value;
	if(opt_yield) 
		sched_yield();
	*pointer = sum;
	
	pthread_mutex_unlock(&mutexd);
}

void spin_add(long long* pointer, long long value)
{
		//fprintf(stderr, "RM: spin add\n");
	while(__sync_lock_test_and_set(&spind, 1)) continue;
	long long sum = *pointer + value;
	if(opt_yield) 
		sched_yield();
	*pointer = sum;
	 __sync_lock_release(&spind);
}

void cas_add(long long* pointer, long long value)
{
		//fprintf(stderr, "RM: cas add\n");
	while(__sync_val_compare_and_swap(&spind, 0, 1)) continue;
	if(opt_yield) 
		sched_yield();
	long long sum = *pointer + value;
	*pointer = sum;
	__sync_val_compare_and_swap(&spind, 1, 0);
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
		args->func(pointer, 1);
	}
	// fprintf(stderr, "RM: mid: %d iterations. %lld sum\n", iterations, *pointer);
	for(i = 0; i < iterations; i++)
	{
		args->func(pointer, -1);
	}
	// fprintf(stderr, "RM: final: %d iterations. %lld sum\n", iterations, *pointer);

	pthread_exit(NULL);
	return NULL;
}


char* namer(int yield, char sync)
{
	char* temp;
	int tot = 6;
	if(sync == 'n') tot += 4;

	if(yield)
	{
		temp = malloc(sizeof(char) * (tot + 6));
		if(temp == NULL)
		{
			fprintf(stderr, "malloc failed\n");
			exit(2);
		}
		strcpy(temp, "add-yield-");
	}
	else
	{
		temp = malloc(sizeof(char) * tot);
		if(temp == NULL)
		{
			fprintf(stderr, "malloc failed\n");
			exit(2);
		}
		strcpy(temp, "add-");
	}

	switch(sync)
	{
		case 'n':
			strcat(temp, "none");
			break;
		case 'm':
			strcat(temp, "m");
			break;
		case 's':
			strcat(temp, "s");
			break;
		case 'c':
			strcat(temp, "c");
			break;
		default:
			fprintf(stderr, "FATAL\n");
			exit(1);
	}

	return temp;
}

int main(int argc, char* argv[])
{
	//struct will go through the struct
	add_func func = add;
	char sync = 'n';

	//setting up and doing the getopt
	int c; 
	int option_index = 0;
	static struct option long_options[] =
	{
		{"threads", 	required_argument, 	0, 't'},
		{"iterations",	required_argument, 	0, 'i'},
		{"yield", 	no_argument,		0, 'y'},
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
				opt_yield = 1;
				break;
			case 's':
				switch(optarg[0])
				{
					case 'm':
						pthread_mutex_init(&mutexd, NULL);
						func = mut_add;
						sync = 'm';
						break;
					case 's':
						func = spin_add;
						sync = 's';
						break;
					case 'c':
						func = cas_add;
						sync = 'c';
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

	//arguments to pass through the threads
	pthread_t threadids[num_threads];
	long long sum = 0;
	struct arg_struct args;
	args.iterations = num_iterations;
	args.pointer = &sum;
	args.func = func;



	//initalizing the timer
	struct timespec timer1;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer1) != 0)
	{
		fprintf(stderr, "failed to get time 1\n");
		exit (2);
	}
	long long counter = 0; //spec told me to do this

	
	//running the threads now
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
	
	//ending the timer
	struct timespec timer2;
	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timer2) != 0)
	{
		fprintf(stderr, "failed to get time 2\n");
		exit (2);
	}



	//the name
	int tot_it = num_threads * num_iterations * 2;
	char* name = namer(opt_yield, sync);

	int f_time = timer2.tv_nsec - timer1.tv_nsec;
	int avg_time = f_time / tot_it;

	fprintf(stderr, "%s,%d,%d,%d,%d,%d,%lld\n", name, num_threads, num_iterations,
		tot_it, f_time, avg_time, sum);
	fprintf(stdout, "%s,%d,%d,%d,%d,%d,%lld\n", name, num_threads, num_iterations,
		tot_it, f_time, avg_time, sum);

	pthread_mutex_destroy(&mutexd);
	free(name);
	return 0;
}
