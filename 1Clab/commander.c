#include "commander.h"
#include "printers.h"
#include "utils.h"
#include <stdio.h> //fprintf
#include <stdlib.h> //exit malloc realloc
#include <unistd.h> //fork execvp dup2
#include <sys/types.h> //wait
#include <sys/wait.h>
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
			//fprintf(stderr, "RM: allocated %d memory, and put %s into cmd[%d]\n", cmdargnum + 1, curr, cmdargnum);
		}
		//rest of the arguments can be added onto the string
		else
		{	
			answer->cmd = (char**)realloc(answer->cmd, sizeof(char*) * (cmdargnum + 1));
			if(answer->cmd == NULL) errmess();
			answer->cmd[cmdargnum] = curr;
			//fprintf(stderr, "RM: allocated %d memory, and put %s into cmd[%d]\n", cmdargnum + 1, curr, cmdargnum);
		}
		
		//fprintf(stderr, "RM: %s\n", curr);
		index++;
		argnum++;
		cmdargnum++;
	}

	answer->cmd = (char**)realloc(answer->cmd, sizeof(char*) * cmdargnum + 1);
	answer->cmd[cmdargnum] = NULL;
	answer->args = cmdargnum;
	//fprintf(stderr, "RM :cmdargnum: %d\n", cmdargnum);

	return 0;
}

int command_do(Command* gotten)
{
	int pid = fork();
	if(pid == 0)
	{
		int i;
		//change the fds to stdin, stdout and stderr
		for(i = 0; i < FDS; i++)
		{
			//fprintf(stderr, "to: %d, from: %d\n", gotten->fds[i], i + 3);
			dup2(curr_fds[gotten->fds[i]], i);
		}
		//close everything else
		for(i = 0; i < curr_fd; i++)
		{
			//fprintf(stderr, "curr_fds: %d, closing: %d\n", curr_fd, i);
			close(curr_fds[i]);
		}

		execvp(gotten->cmd[0], gotten->cmd);

		//will continue here if execvp didn't execute
	    	fprintf(stderr, "invalid command\n");
		exit(1);
	}
	//fprintf(stderr, "pid: %d, going into index %d, allocating %d mem\n", pid, num_proc, num_proc + 2);
	gotten->pid = pid;

	//now put it in the array of processes
	curr_proc[num_proc] = *gotten;
	num_proc++;
	curr_proc = (Command*)realloc(curr_proc, sizeof(Command) * (num_proc + 1));
	if(curr_proc == NULL) errmess();

	//free(gotten->cmd);
	return 0;
}

int command_list(Command* gotten)
{
	int i;
	for(i = 0; i < gotten->args + 1; i++)
	{
		fprintf(stderr, "RM: cmd[%d] = %s\n", i, gotten->cmd[i]);
	}
	return 0;
}

void command_free()
{
	int i;
	for(i = 0; i < num_proc; i++)
	{
		//fprintf(stderr, "RM: process #%d, pid: %d\n", i, curr_proc[i].pid);
		//stringer2(curr_proc[i].cmd, curr_proc[i].args);
		free(curr_proc[i].cmd);
	}
	free(curr_proc);
	num_proc = 0;
}


int waiter()
{
	int status;
	int return_status = 0;
	int i;
	for(i = 0; i < num_proc; i++)
	{
		//fprintf(stderr, "starting wait for pid: %d\n", curr_proc[i].pid);
		int pid = waitpid(curr_proc[i].pid, &status, 0);
		if(pid < 0)
		{
			fprintf(stderr, "FATAL: wait error\n");
			exit(1);
		}
		
		//fprintf(stderr, "status: %d, pid: %d", status, pid);
		if(WIFSIGNALED(status))
		{
			fprintf(stdout, "signal %d", WTERMSIG(status));	
			return_status = max(WTERMSIG(status) + 128, return_status);
		}
		else if(WIFEXITED(status))
		{
			fprintf(stdout, "exit %d", WEXITSTATUS(status));
			return_status = max(WEXITSTATUS(status), return_status);
		}
		else
		{
			fprintf(stderr, "not signaled or exited?\n");
		}
		stringer2(curr_proc[i].cmd, curr_proc[i].args);
		//fprintf(stderr, "return_status: %d\n", return_status);
	}
	//freeing the array, then remake it
	command_free();
	curr_proc = (Command*)malloc(sizeof(Command));
	if(curr_fds == NULL) errmess();

	//fprintf(stderr, "Done\n");
	return return_status;
}

