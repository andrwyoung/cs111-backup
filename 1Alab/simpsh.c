//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h> //fprinf
#include <getopt.h>  //getopt_long
#include <stdlib.h>  //exit
#include <sys/types.h> //open
#include <sys/stat.h> 
#include <fcntl.h> 
#include <string.h> //strlen
#include <ctype.h> //isdigit
#define FDS 3

int verbose_flag = 0; //is verbose on?
int file_flags = 0; //file flags open()
int curr_fd = 0;	//number of fds currently open
int exit_status = 0;
int* curr_fds;

typedef struct Command {
	int fds[FDS];
	char** cmd;
	int args;
} Command;

void errmess()
{
	fprintf(stderr, "memory allocation failure, exiting\n");
	exit(1);
}

void stringer0(char option[])
{
	printf("--%s\n", option);
	fflush(stdout);
}

//string outputter for verbose options with 1 argument
void stringer1(const char option[], const char arguments[])
{
	printf("--%s %s\n", option, arguments);
	fflush(stdout);
}

//string outputter for --command
void stringer2(Command* gotten)
{
	int* fds = gotten->fds;
	char** cmd = gotten->cmd;
	printf("---command");
	int i;
	for(i = 0; i < FDS; i++)
	{
		printf(" %d", fds[i]);
	}
	for(i = 0; i < gotten->args; i++)
	{
		printf(" %s", cmd[i]);
	}
	printf("\n");
	fflush(stdout);
}



//check if string is int, then convert
//-1 if not int
int stoi(char* string)
{
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
			return -1;
	}
	return atoi(string);
}



//open new file with said flags
void new_open(char file[], int flag)
{
	int fd = open(file, flag | file_flags);

	curr_fd++;
	curr_fds = (int*)realloc(curr_fds, sizeof(int) * curr_fd);
	curr_fds[curr_fd - 1] = fd;
	

	if(fd < 0)
	{
		fprintf(stderr, "open fail for %s\n", file);
		exit_status = 1;
	}
	
	//fprintf(stderr, "opened file %s, fd: %d\n", file, fd);
}


void fd_print()
{
	int i;
	for(i = 0; i < curr_fd; i++)
	{
		fprintf(stderr, "%d\n", curr_fds[i]);
	}
}



int command_parse(int argc, char* argv[], Command* answer)
{
	int index = optind - 1;
	int argnum = 0;
	int cmdargnum = -FDS; //makes it easier for me

	//this is all to get and check arguments
	while(1)
	{
		//fprintf(stderr, "index: %d, optind: %d, argc: %d, argnum: %d, cmdargnum: %d\n", index, optind, argc, argnum, cmdargnum);
		char* curr = argv[index];
		if(index >= argc || (curr[0] == '-' && curr[1] == '-'))
		{
			if(argnum < FDS + 1) 
			{
				fprintf(stderr, "--command: at least 4 arguments required (--help for usage)\n");
				return 1;
			}
			optind = index;
			break;
		}


		//first 3 arguments must be numbers
		if(argnum < FDS)
		{
			answer->fds[argnum] = stoi(curr);
			int fd = answer->fds[argnum];
			if(fd < 0 || fd >= curr_fd || curr_fds[fd] < 0)
			{
				fprintf(stderr, "invalid file descpriptor\n");
				exit(1);
			}
			if (fd < 0)
			{
				fprintf(stderr, "--command: argument %d must be a number (--help for usage)\n", argnum + 1);
				return 1;
			}
		}
		//fourth argument is the start of the command. so allocate it
		else if(argnum == FDS)
		{
			answer->cmd = (char**)malloc(sizeof(char*));
			if(answer->cmd == NULL) errmess();
			answer->cmd[0] = curr;
			//fprintf(stderr, "allocated %d memory\n", cmdargnum + 1);
		}
		//rest of the arguments can be added onto the string
		else
		{	
			answer->cmd = (char**)realloc(answer->cmd, sizeof(char*) * (cmdargnum + 1));
			if(answer->cmd == NULL) errmess();
			answer->cmd[cmdargnum] = curr;
			//fprintf(stderr, "allocated %d memory\n", cmdargnum + 1);
		}
		
		//fprintf(stderr, "%s\n", curr);
		index++;
		argnum++;
		cmdargnum++;
	}

	answer->cmd = (char**)realloc(answer->cmd, sizeof(char*) * cmdargnum);
	answer->cmd[cmdargnum] = NULL;
	answer->args = cmdargnum;
	//fprintf(stderr, "cmdargnum: %d\n", cmdargnum);

	return 0;
}

int command_do(Command* gotten)
{
	if(fork() == 0)
	{
		if(verbose_flag) stringer2(gotten);

		int i;
		for(i = 0; i < FDS; i++)
		{
			//fprintf(stderr, "to: %d, from: %d\n", gotten->fds[i], i + 3);
			dup2(curr_fds[gotten->fds[i]], i);
		}

		execvp(gotten->cmd[0], gotten->cmd);

		//will continue here if execvp didn't execute
	    fprintf(stderr, "invalid command\n");
		exit(1);
	}

	//free(gotten->cmd);
	return 0;
}

int command_free(Command* gotten)
{
	int i;
	for(i = 0; i < gotten->args; i++)
	{
		fprintf(stderr, "%s\n", gotten->cmd[i]);
	}
	return 0;
}



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
		{0, 0, 0, 0,}

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
			case 20: //rdonly
				if(verbose_flag) stringer1("rdonly", optarg);
				new_open(optarg, O_RDONLY);
				break;
			case 22: //wronly
				if(verbose_flag) stringer1("wronly", optarg);
				new_open(optarg, O_WRONLY);
				break;
			case 24: //command
				{
					Command gotten;
					if(command_parse(argc, argv, &gotten)) 
							exit_status = 1;
					else
						command_do(&gotten);
					//command_free(&gotten);
					//free(gotten->cmd)
				}
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
		
	free(curr_fds);	
	return exit_status;
}
