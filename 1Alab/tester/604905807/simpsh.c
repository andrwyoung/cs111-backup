//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h> //fprinf
#include <getopt.h>  //getopt_long
#include <stdlib.h>  //exit
#include <string.h> //strerror atoi itoa
#include <errno.h> //errno
#include <sys/types.h> //open
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> //read and write
#include <ctype.h> //isdigit

#define FDS 3 //how many file descripors needed for command

//flags
static int verbose_flag = 0;
static int file_flags = 0;
static int curr_fd = 0;	
static int exit_status = 0;

//printing messages
void printusage()
{
	fprintf(stderr, "remember to print usage information\n");
}

void errmess()
{
	fprintf(stderr, "memory allocation failure, exiting\n");
	exit(1);
}

//check if string is int, then convert
//-1 if not int
int stoi(char* string)
{
	for(int i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
			return -1;
	}
	int answer = atoi(string);
	if(answer >= curr_fd) 
	{
		fprintf(stderr, "hmm\n");
		exit(1);
	}
	return atoi(string);
}






//open new file with said flags
int new_open(char file[], int flag)
{
	close(curr_fd);
	curr_fd++;
	int fd = open(file, flag | file_flags);
	if(fd < 0)
	{
		fprintf(stderr, "open fail for %s: %s\n", file, strerror(errno));
		exit(1);
	}
	
	//fprintf(stderr, "opened file %s, fd: %d\n", file, fd);
	return fd;
}

void stringer0(char option[])
{
	printf("--%s\n", option);
	fflush(stdout);
}

//string outputter for verbose options with 1 argument
void stringer1(char option[], char arguments[])
{
	printf("--%s %s\n", option, arguments);
	fflush(stdout);
}

//string outputter for --command
void stringer2(int fds[], char* cmd)
{
	printf("--command %d %d %d %s\n", fds[0], fds[1], fds[2], cmd);
	fflush(stdout);
}

//do the --command with correctly formatted input
int commander(int fds[], char* cmd)
{	
	if(fork() == 0)
	{
		//fprintf(stderr, "pid: %d\n", getpid());
		//fprintf(stderr, "forked!\n");
		if(verbose_flag) stringer2(fds, cmd);

		for(int i = 0; i < FDS; i++)
		{
			dup2(fds[i], i);
		}

	
		//fprintf(stdout, "output!\n");
		//fprintf(stderr, "error!\n");

		system(cmd);
		exit(0);
	}


	return 0;
}

int main(int argc, char* argv[])
{
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

	//reading the options
	while(1)
	{
		c = getopt_long(argc, argv, "", long_options, &option_index);

		if(c == -1) 
			break;
		// fprintf(stderr, "---c: %d, optind: %d, option_index: %d, optarg: %s\n", c, option_index, optind, optarg);

		int index; //for --command
		switch(c)
		{
			case 20: //rdonly
				if(verbose_flag) stringer1("rdonly", optarg);
				new_open(optarg, O_RDONLY);
				break;
			case 22: //wronly
				if(verbose_flag) stringer1("wronly", optarg);
				new_open(optarg, O_WRONLY);
				break;
			case 24: //command
				index = optind - 1;
				int argnum = 0;

				int fail = 0; //supports continuing despite bad arguments
				int fds[FDS]; //to hold onto fds
				char* result = NULL; //to hold onto the cmd + args
				//this is all to get and check arguments
				while(1)
				{
					//end cases
					//fprintf(stderr, "index: %d, argc: %d, argnum: %d, optind: %d\n", index, argc, argnum, optind);
					char* curr = argv[index];
					if(index >= argc || curr[0] == '-')
					{
						if(argnum < FDS + 1) 
						{
							//fprintf(stderr, "--command: at least 4 arguments required (--help for usage)\n");
							fail = 1;
						}
						optind = index;
						break;
					}

					//if statment to preserve order of arguments
					//first 3 arguments must be numbers
					if(argnum < FDS)
					{
						fds[argnum] = stoi(curr);
						if (fds[argnum] < 0)
						{
							//fprintf(stderr, "--command: argument %d must be a number (--help for usage)\n", argnum + 1);
							fail = 1;
							break; //may be problem: not moving optind
						}
					}
					//fourth argument is a string. so allocate it
					else if(argnum == FDS)
					{
						result = malloc(strlen(curr));
						if(result == NULL) errmess();
						strcat(result, curr);
					}
					//rest of the arguments can be added onto the string
					else
					{
						result = (char*)realloc(result, sizeof(int) * (strlen(curr) + strlen(result)));
						if(result == NULL) errmess();
						strcat(result, " ");
						strcat(result, curr);
					}
					
					//fprintf(stderr, "%s\n", curr);
					index++;
					argnum++;
				}
				if(!fail) commander(fds, result); //now do the command
				if(result != NULL) //free memory. just in case
				{
					free(result);
					result = NULL;
				}
				break;
			case 31:
				if(verbose_flag) stringer0("verbose");
				verbose_flag = 1;
				break;
			case '?':
				exit(1);
		}

		//fprintf(stderr, "verbose_flag: %d\n", verbose_flag);
		//printf("pid: %d\n", getpid());
	}
	//fprintf(stderr, "argc: %d, optind: %d\n", argc, optind);
			
	return exit_status;
}
