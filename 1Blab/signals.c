#include "signals.h"
#include <stdio.h>
#include <stdlib.h> //exit

void aborter()
{
	int* p = 0;
	*p = 10;
}

void catcher(int signum)
{
	fprintf(stderr, "%d caught\n", signum);
	exit(signum);
}
