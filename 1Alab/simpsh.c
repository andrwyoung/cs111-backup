//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h> //fprinf
#include <getopt.h>  //getopt_long
#include <stdlib.h>  //exit
#include <string.h> //strerror
#include <errno.h> //errno
#include <sys/types.h> //open
#include <sys/stat.h> 
#include <fcntl.h> 

//flags
static int verbose_flag = 0;
static int file_flags = 0;

void printusage()
{
	fprintf(stderr, "remember to print usage information\n");
}


int new_open(char file[], int flag)
{
	int fd = open(file, flag | file_flags);
	if(fd < 0)
	{
		fprintf(stderr, "open fail for %s: %s\n", file, strerror(errno));
	}
		
	// fprintf(stderr, "opened file %s, fd: %d\n", file, fd);
	return fd;
}


int main(int argc, char* argv[])
{
	fprintf(stderr, strcat("rdonly\n", optarg));

	int c; //return value of that option
	int option_index = 0;
	static struct option long_options[] = 
	{
		//file flags
		//not starting at zero: don't know if that has any special meaning in getopt_long
		{"append",	 	no_argument,		0, 1},
		{"cloexec",	 	no_argument,		0, 2},
		{"creat",	 	no_argument,		0, 3},
		{"directory",	no_argument,		0, 4},
		{"dsync",	 	no_argument,		0, 5},
		{"excl",	 	no_argument,		0, 6},
		{"nofollow",	no_argument,		0, 7},
		{"nonblock",	no_argument,		0, 8},
		{"rsync",	 	no_argument,		0, 9},
		{"sync",	 	no_argument,		0, 10},
		{"trunc",	 	no_argument,		0, 11},
		
		//open file descriptors
		{"rdonly",		required_argument,	0, 20}, 
		{"rdwr",		required_argument,	0, 21}, 
		{"wronly",		required_argument,	0, 22}, 
		{"pipe",		no_argument,		0, 23}, 

		//subcommands
		{"command",		required_argument,	0, 24},
		{"wait", 		no_argument,		0, 25},

		//miscellaneous options
		{"close", 		required_argument,	0, 30},
		{"verbose", 	no_argument,		0, 31},
		{"profile", 	no_argument,		0, 32},
		{"abort", 		no_argument,		0, 33},
		{"catch", 		required_argument,	0, 34},
		{"ignore", 		required_argument,	0, 35},
		{"default", 	required_argument,	0, 36},
		{"pause", 		no_argument,		0, 37},

		//help option
		{"help",		no_argument,		0, 40}

	};
	
	// //checking if valid arguments?
	// while(1)
	// {
	// 	c = getopt_long(argc, argv, "", long_options, &option_index);
	// 	if(c == -1) 
	// 		break;

	// 	if(c == '?')
	// 	{
	// 		printusage();
	// 		exit(1);
	// 	}
	// }
	// optind = 0;



	//reading the options
	while(1)
	{
		c = getopt_long(argc, argv, "", long_options, &option_index);

		if(c == -1) 
			break;
		// fprintf(stderr, "---c: %d, optind: %d, option_index: %d, optarg: %s\n", c, option_index, optind, optarg);

		switch(c)
		{
			case 20: //rdonly
				char input[] = 
				write()
				new_open(optarg, O_RDONLY);
				break;
			case 22: //wronly
				new_open(optarg, O_WRONLY);
				break;
			case 24: //command

				break;
			case 31:
				verbose_flag = 1;
				break;
		}

		// fprintf(stderr, "verbose_flag: %d\n", verbose_flag);
	}



			
	return 0;
}
