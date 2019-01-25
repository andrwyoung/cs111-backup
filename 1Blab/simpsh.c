//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h> //fprintf
#include <getopt.h>  //getopt_long
#include <sys/types.h> //open+flags
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h> //malloc

#include "printers.h"
#include "commander.h"
#include "filers.h"

int verbose_flag = 0; //is verbose on?
int exit_status = 0;

int file_flags = 0; //flags to use for open()
int curr_fd = 0;	//number of fds currently open
int* curr_fds; //current file descriptor


int main(int argc, char* argv[])
{
	int c; //return value of that option
	int option_index = 0;
	curr_fds = (int*)malloc(sizeof(int));
	if(curr_fds == NULL) errmess();

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
		{"help",		no_argument,		0, 40},
		{0, 			0, 					0, 0,}

	};

	//reading the options
	while(1)
	{
		c = getopt_long(argc, argv, "", long_options, &option_index);
		if(c == -1) 
			break;
		//fprintf(stderr, "---c: %d, optind: %d, option_index: %d, optarg: %s\n", c, option_index, optind, optarg);

		int index; //for --command
		switch(c)
		{
			case 1: //append
				if(verbose_flag) stringer0("append");
				file_flags |= O_APPEND;
				break;
			case 2: //cloexec
				if(verbose_flag) stringer0("cloexec");
				file_flags |= O_CLOEXEC;
				break;
			case 3: //creat
				if(verbose_flag) stringer0("creat");
				file_flags |= O_CREAT;
				break;
			case 4: //directory
				if(verbose_flag) stringer0("directory");
				file_flags |= O_DIRECTORY;
				break;
			case 5: //dsync
				if(verbose_flag) stringer0("dsync");
				file_flags |= O_DSYNC;
				break;
			case 6: //excl
				if(verbose_flag) stringer0("excl");
				file_flags |= O_EXCL;
				break;
			case 7: //nofollow
				if(verbose_flag) stringer0("nofollow");
				file_flags |= O_NOFOLLOW;
				break;
			case 8: //nonblock
				if(verbose_flag) stringer0("nonblock");
				file_flags |= O_NONBLOCK;
				break;
			// case 9: //rsync
			// 	if(verbose_flag) stringer0("rsync");
			// 	file_flags |= O_RSYNC;
			// 	break;
			case 10: //sync
				if(verbose_flag) stringer0("sync");
				file_flags |= O_SYNC;
				break;
			case 11: //trunc
				if(verbose_flag) stringer0("trunc");
				file_flags |= O_TRUNC;
				break;


			case 20: //rdonly
				if(verbose_flag) stringer1("rdonly", optarg);
				new_open(optarg, O_RDONLY);
				break;
			case 21: //rdwr
				if(verbose_flag) stringer1("rdwr", optarg);
				new_open(optarg, O_RDWR);
				break;
			case 22: //wronly
				if(verbose_flag) stringer1("wronly", optarg);
				new_open(optarg, O_WRONLY);
				break;
			case 23: //pipe
				if(verbose_flag) stringer0("piper");
				piper();
				break;


			case 24: //command
				{
					Command gotten;
					if(command_parse(argc, argv, &gotten)) 
							exit_status = 1;
					else
						command_do(&gotten);
					//command_list(&gotten);
					//free(gotten->cmd)
				}
				break;
			case 35: //wait
				break;


			case 31:
				if(verbose_flag) stringer0("verbose");
				verbose_flag = 1;
				break;



			case '?':
				fprintf(stderr, "bad option\n");
				exit_status = 1;
				break;
		}

		//fprintf(stderr, "verbose_flag: %d\n", verbose_flag);
		//printf("pid: %d\n", getpid());
	}
	//fprintf(stderr, "argc: %d, optind: %d\n", argc, optind);
	fd_print();
		
	free(curr_fds);	
	return exit_status;
}
