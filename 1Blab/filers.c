#include "filers.h"
#include "utils.h"
#include <stdio.h> //fprintf
#include <stdlib.h> //realloc
#include <unistd.h> //pipe
#include <sys/types.h> //open
#include <sys/stat.h>
#include <fcntl.h>



//open new file with said flags
int opener(char file[], int flag)
{
	int fd = open(file, flag | file_flags);
	file_flags = 0; //what if open failed though?

	curr_fd++;
	curr_fds = (int*)realloc(curr_fds, sizeof(int) * curr_fd);
	curr_fds[curr_fd - 1] = fd;
	if(fd < 0)
	{
		fprintf(stderr, "open fail for %s\n", file);
		return 1;
	}
	fprintf(stderr, "RM: opened file %s, fd: %d\n", file, fd);
	return 0;
}

int closer(char* string)
{
	int fd = stoi(string);
	if(fd < 0 || fd > curr_fd - 1)
	{
		fprintf(stderr, "invalid file descriptor: %s\n", string);
		return 1;
	}
	
	if(close(curr_fds[fd]) < 0)
	{
		fprintf(stderr, "FATAL: failed to close %d\n", fd); //never should happen
		return 1;
	}

	fprintf(stderr, "RM: closed fd %d\n", fd);
	curr_fds[fd] = -1;
	return 0;
}

int piper()
{
	curr_fd += 2;
	curr_fds = (int*)realloc(curr_fds, sizeof(int) * curr_fd);
	if(pipe(curr_fds + curr_fd - 2) < 0)
	{
		fprintf(stderr, "pipe failed to open\n");
		curr_fds[curr_fd - 2] = -1;
		curr_fds[curr_fd - 1] = -1;
		return 1;
	}
	return 0;
}

void fd_print()
{
	int i;
	for(i = 0; i < curr_fd; i++)
	{
		fprintf(stderr, "%d\n", curr_fds[i]);
	}
}