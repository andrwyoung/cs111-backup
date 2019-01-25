#include "commander.h"
#include "printers.h"
#include <stdio.h> //fprintf
#include <stdlib.h> //exit malloc realloc
#include <unistd.h> //fork execvp dup2
#define FDS 3 //makes it easier for me


int command_parse(int argc, char* argv[], Command* answer)
{
	extern int optind;
	int index = optind - 1;
	int argnum = 0;
	int cmdargnum = -FDS; //makes it easier for me

	//this is all to get and check arguments
	while(1)
	{
		//fprintf(stderr, "RM: index: %d, optind: %d, argc: %d, argnum: %d, cmdargnum: %d\n", index, optind, argc, argnum, cmdargnum);
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
			//fprintf(stderr, "RM: allocated %d memory\n", cmdargnum + 1);
		}
		//rest of the arguments can be added onto the string
		else
		{	
			answer->cmd = (char**)realloc(answer->cmd, sizeof(char*) * (cmdargnum + 1));
			if(answer->cmd == NULL) errmess();
			answer->cmd[cmdargnum] = curr;
			//fprintf(stderr, "RM: allocated %d memory\n", cmdargnum + 1);
		}
		
		//fprintf(stderr, "RM: %s\n", curr);
		index++;
		argnum++;
		cmdargnum++;
	}

	answer->cmd = (char**)realloc(answer->cmd, sizeof(char*) * cmdargnum);
	answer->cmd[cmdargnum] = NULL;
	answer->args = cmdargnum;
	//fprintf(stderr, "RM :cmdargnum: %d\n", cmdargnum);

	return 0;
}

int command_do(Command* gotten)
{
	if(fork() == 0)
	{
		if(verbose_flag) stringer2(gotten->fds, gotten->cmd, gotten->args);

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

int command_list(Command* gotten)
{
	int i;
	for(i = 0; i < gotten->args; i++)
	{
		fprintf(stderr, "%s\n", gotten->cmd[i]);
	}
	return 0;
}



int wait()
{
	return 0;
}

