//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int c; //return value of that option
	int option_index = 0;
	static struct option long_options[] = 
	{
		//file flags
		{"trunc",	 no_argument,		0, 0},
		{"append",	 no_argument,		0, 1},
		{"cloexec",	 no_argument,		0, 2},
		{"creat",	 no_argument,		0, 3},
		{"directory",	 no_argument,		0, 4},
		{"dsync",	 no_argument,		0, 5},
		{"excl",	 no_argument,		0, 6},
		{"nofollow",	 no_argument,		0, 7},
		{"nonblock",	 no_argument,		0, 8},
		{"rsync",	 no_argument,		0, 9},
		{"sync",	 no_argument,		0, 10},
		
		//open file descriptors
		{"rdonly",	required_argument,	0, 20}, 
		{"rdwr",	required_argument,	0, 21}, 
		{"wronly",	required_argument,	0, 22}, 
		{"pipe",	no_argument,		0, 23}, 

		//subcommands
		{"command",	required_argument,	0, 24},
		{"wait", 	no_argument,		0, 25},

		//miscellaneous options
		{"close", 	required_argument,	0, 30},
		{"verbose", 	no_argument,		0, 31},
		{"profile", 	no_argument,		0, 32},
		{"abort", 	no_argument,		0, 33},
		{"catch", 	required_argument,	0, 34},
		{"ignore", 	required_argument,	0, 35},
		{"default", 	required_argument,	0, 36},
		{"pause", 	no_argument,		0, 37},

	};
	
	while(1)
	{
		getopt_long(argc, argv, "r:w:", long_options, &option_index);
		if(c == -1) break;

		if(c == '?')
		{
			fprintf(stderr, "bad usage");
			exit(1);
		}
	}
			
	return 0;
}
