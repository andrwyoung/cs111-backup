#include <stdio.h> //fprintf
#include <fcntl.h> // open
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> //dup write read close
#include <stdlib.h> //exits

//change fd1 (stdout) to go to file[] instead
void chout(int* file)
{
	*file = 2;
}




int main()
{
	int x = 3;
	fprintf(stderr, "%d\n", x);
	chout(&x);
	fprintf(stderr, "%d\n", x);
	return 0;
}