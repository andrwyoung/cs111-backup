#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
	char** cmd;
	cmd = (char**)malloc(sizeof(char*) * 2);
	cmd[0] = "ls";
	cmd[1] = NULL;

	execvp(cmd[0], cmd);
	fprintf(stderr, "hi\n");

	return 0;
}