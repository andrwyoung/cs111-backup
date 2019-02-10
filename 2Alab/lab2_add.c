//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils.h"

struct arg_struct {
	long long* pointer;
	long long value;
};

void add(long long* pointer, long long value)
{
	fprintf(stderr, "1\n");
	long long sum = *pointer + value;
	*pointer = sum;
}

int main(int argc, char* argv[])
{
	//remember to start the counter
	long long counter = 0;
	char* name = "add-none";

	int c;
	int option_index = 0;
	static struct option long_options[] =
	{
		{"threads", 	required_argument, 0, 't'},
		{"iterations",	required_argument, 0, 'i'},
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
					fprintf(stderr, "RM: %s threads\n", optarg);
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
					fprintf(stderr, "RM: %s iterations\n", optarg);
					num_iterations = num;
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

	//running the threads now
	pthread_t threadids[num_iterations];
	long long sum = 0;
	long long value = 1;
	
	int i;
	for(i = 0; i < num_threads; i++)
	{
		
	}

	//final printing
	fprintf(stdout, "%s,%d,%d,%d,%d,%d,%d\n", name, num_threads, num_iterations,
		num_threads * num_iterations * 2, -1, -1, -1);
	return 0;
}
