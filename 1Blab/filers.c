#include "filers.h"
#include <stdio.h> //fprintf
#include <stdlib.h> //realloc
#include <sys/types.h> //open
#include <sys/stat.h>
#include <fcntl.h>


//open new file with said flags
void new_open(char file[], int flag)
{
	int fd = open(file, flag | file_flags);
	file_flags = 0;

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