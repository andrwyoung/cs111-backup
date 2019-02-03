//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h> //fprintf
#include <getopt.h>  //getopt_long
#include <sys/types.h> //open+flags
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h> //malloc
#include <unistd.h>
#include <signal.h>

#include "printers.h"
#include "commander.h"
#include "filers.h"
#include "utils.h"
#include "signals.h"
#include "profiler.h"

int verbose_flag = 0; //is verbose on?
int profile_flag = 0;

int exit_status = 0;
int* commands; //list of commands currently running

int file_flags = 0; //flags to use for open()
int curr_fd = 0; //number of fds currently open
int* curr_fds; //current file descriptor

int num_proc = 0;
Command* curr_proc;

int main(int argc, char* argv[])
{
	int c; //return value of that option
	int option_index = 0;
	curr_fds = (int*)malloc(sizeof(int));
	if(curr_fds == NULL) errmess();
	curr_proc = (Command*)malloc(sizeof(Command));
	if(curr_fds == NULL) errmess();

	static struct option long_options[] = 
	{
		//file flags
		//not starting at zero: don't know if that has any special meaning in getopt_long
		{"append",	 	no_argument,		0, 1},
		{"cloexec",	 	no_argument,		0, 2},
		{"creat",	 	no_argument,		0, 3},
		{"directory",		no_argument,		0, 4},
		{"dsync",	 	no_argument,		0, 5},
		{"excl",	 	no_argument,		0, 6},
		{"nofollow",		no_argument,		0, 7},
		{"nonblock",		no_argument,		0, 8},
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
		{"verbose", 		no_argument,		0, 31},
		{"profile", 		no_argument,		0, 32},
		{"abort", 		no_argument,		0, 33},
		{"catch", 		required_argument,	0, 34},
		{"ignore", 		required_argument,	0, 35},
		{"default", 		required_argument,	0, 36},
		{"pause", 		no_argument,		0, 37},

		//help option
		{"help",		no_argument,		0, 40},
		{0, 			0, 			0, 0,}

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
				if(profile_flag) profiler_start();
				file_flags |= O_APPEND;
				if(profile_flag) profiler_end("append");
				break;
			case 2: //cloexec
				if(verbose_flag) stringer0("cloexec");
				if(profile_flag) profiler_start();
				file_flags |= O_CLOEXEC;
				if(profile_flag) profiler_end("cloexec");
				break;
			case 3: //creat
				if(verbose_flag) stringer0("creat");
				if(profile_flag) profiler_start();
				file_flags |= O_CREAT;
				if(profile_flag) profiler_end("creat");
				break;
			case 4: //directory
				if(verbose_flag) stringer0("directory");
				if(profile_flag) profiler_start();
				file_flags |= O_DIRECTORY;
				if(profile_flag) profiler_end("directory");
				break;
			case 5: //dsync
				if(verbose_flag) stringer0("dsync");
				if(profile_flag) profiler_start();
				file_flags |= O_DSYNC;
				if(profile_flag) profiler_end("dsync");
				break;
			case 6: //excl
				if(verbose_flag) stringer0("excl");
				if(profile_flag) profiler_start();
				file_flags |= O_EXCL;
				if(profile_flag) profiler_end("excl");
				break;
			case 7: //nofollow
				if(verbose_flag) stringer0("nofollow");
				if(profile_flag) profiler_start();
				file_flags |= O_NOFOLLOW;
				if(profile_flag) profiler_end("nofollow");
				break;
			case 8: //nonblock
				if(verbose_flag) stringer0("nonblock");
				if(profile_flag) profiler_start();
				file_flags |= O_NONBLOCK;
				if(profile_flag) profiler_end("nonblock");
				break;
			//case 9: //rsync
			//	if(verbose_flag) stringer0("rsync");
			//	if(profile_flag) profiler_start();
			//	file_flags |= O_RSYNC;
			//	if(profile_flag) profiler_end("rsync");
			//	break;
			case 10: //sync
				if(verbose_flag) stringer0("sync");
				if(profile_flag) profiler_start();
				file_flags |= O_SYNC;
				if(profile_flag) profiler_end("sync");
				break;
			case 11: //trunc
				if(verbose_flag) stringer0("trunc");
				if(profile_flag) profiler_start();
				file_flags |= O_TRUNC;
				if(profile_flag) profiler_end("trunc");
				break;


			case 20: //rdonly
				if(verbose_flag) stringer1("rdonly", optarg);
				if(profile_flag) profiler_start();
				exit_status = max(exit_status, opener(optarg, O_RDONLY));
				if(profile_flag) profiler_end("rdonly");
				break;
			case 21: //rdwr
				if(verbose_flag) stringer1("rdwr", optarg);
				if(profile_flag) profiler_start();
				exit_status = max(exit_status, opener(optarg, O_RDWR));
				if(profile_flag) profiler_end("rdwr");
				break;
			case 22: //wronly
				if(verbose_flag) stringer1("wronly", optarg);
				if(profile_flag) profiler_start();
				exit_status = max(exit_status, opener(optarg, O_WRONLY));
				if(profile_flag) profiler_end("wronly");
				break;
			case 23: //pipe
				if(verbose_flag) stringer0("pipe");
				if(profile_flag) profiler_start();
				exit_status = max(exit_status, piper());
				if(profile_flag) profiler_end("pipe");
				break;


			case 24: //command
				{
					Command gotten;
					if(command_parse(argc, argv, &gotten)) 
						exit_status = 1;
					else
					{
						if(verbose_flag) 
						{
							printf("--command");
							int i;
							for(i = 0; i < 3; i++)
								printf(" %d", gotten.fds[i]);
							stringer2(gotten.cmd, gotten.args);
						}
						if(profile_flag) profiler_start();
						command_do(&gotten);
						if(profile_flag) profiler_end("command");
					}
					//command_list(&gotten);
					//command_free();
				}
				break;
			case 25: //wait
				if(verbose_flag) stringer0("wait");
				if(profile_flag) 
				{
					profiler_start();
				}	
				exit_status = max(exit_status, waiter());
				//fprintf(stderr, "status: %d\n", exit_status);
				if(profile_flag) 
				{
					struct rusage usage;
					getrusage(RUSAGE_CHILDREN, &usage);
					
					printf("children\t");
					printf("user: %lds %dus ", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
					printf("system: %lds %dus\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
					fflush(stdout);
					profiler_end("wait");
				}
				break;


			case 30: //close N
				if(verbose_flag) stringer1("close", optarg);
				if(profile_flag) profiler_start();
				exit_status = max(exit_status, closer(optarg));
				if(profile_flag) profiler_end("close");
				break;
			case 31: //verbose
				if(verbose_flag) stringer0("verbose");
				if(profile_flag) profiler_start();
				verbose_flag = 1;
				if(profile_flag) profiler_end("verbose");
				break;
		
			case 32: //profile
				if(verbose_flag) stringer0("profile");
				if(profile_flag) profiler_start();
				if(profile_flag) profiler_end("profile");
				profile_flag = 1;
				break;
			
			
			case 33: //abort
				if(verbose_flag) stringer0("abort");
				if(profile_flag) profiler_start();
				aborter();
				if(profile_flag) profiler_end("abort");
				break;
			case 34: //catch N
				if(verbose_flag) stringer1("catch", optarg);
				if(profile_flag) profiler_start();
				{
					int num = stoi(optarg);
					if(num < 0) break;
					signal(num, catcher);
				}
				if(profile_flag) profiler_end("catch");
				break;
			case 35: //ignore N
				if(verbose_flag) stringer1("ignore", optarg);
				if(profile_flag) profiler_start();
				{
					int num = stoi(optarg);
					if(num < 0) break;
					signal(num, SIG_IGN);
				}
				if(profile_flag) profiler_end("ignore");
				break;
			case 36: //default N
				if(verbose_flag) stringer1("default", optarg);
				if(profile_flag) profiler_start();
				{
					int num = stoi(optarg);
					if(num < 0) break;
					signal(num, SIG_DFL);
				}
				if(profile_flag) profiler_end("default");
				break;
			case 37: //pause
				if(verbose_flag) stringer0("pause");
				if(profile_flag) profiler_start();
				pause();
				if(profile_flag) profiler_end("pause");
				break;

			case '?':
				fprintf(stderr, "bad option\n");
				exit_status = max(exit_status, 1);
				break;

			default:
				fprintf(stderr, "FATAL"); //never supposed to happen
		}

		//fprintf(stderr, "verbose_flag: %d\n", verbose_flag);
		//printf("pid: %d\n", getpid());
	}
	//fprintf(stderr, "argc: %d, optind: %d\n", argc, optind);
	//fd_print();
	command_free();	
	free(curr_fds);	
	if(exit_status >= 128)
	{
		signal(exit_status - 128, SIG_DFL);
		raise(exit_status - 128);
	}
	return exit_status;
}
