#include "printers.h"
#include <stdio.h>
#include <stdlib.h> //exit



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
void stringer2(int* fds, char** cmd, int args)
{
	printf("---command");
	int i;
	for(i = 0; i < 3; i++)
	{
		printf(" %d", fds[i]);
	}
	for(i = 0; i < args; i++)
	{
		printf(" %s", cmd[i]);
	}
	printf("\n");
	fflush(stdout);
}

