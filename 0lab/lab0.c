//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h> //fprintf
#include <getopt.h> //getopt
#include <fcntl.h> // open
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> //dup write read close
#include <stdlib.h> //exit
#include <signal.h> //signal
#include <string.h> 
#include <errno.h>
#define BUFF_SIZE 100 

//change fd0 (stdinn) to get from file[] instead
int chin(char file[])
{
	int fd0 = open(file, O_RDONLY);
	if(fd0 >= 0)
	{
		close(0);
		dup(fd0);
		close(fd0);
		return 0;
	}
	else
	{
		fprintf(stderr,  "failed to open %s: %s\n", file, strerror(errno));
		return 1;
	}
}

//change fd1 (stdout) to go to file[] instead
int chout(char file[])
{
	int fd1 = creat(file, 0644);
	if (fd1 >= 0)
	{
		close(1);
		dup(fd1);
		close(fd1);
		return 0;
	}
	else
	{
		fprintf(stderr,  "failed to create %s: %s\n", file, strerror(errno));
		return 1;
	}
}

//set up a signal handler for segmentation faults
void catcher()
{
	fprintf(stderr, "caught sementation fault\n");
	exit(4);
}

//force a segmentation fault
void segfault()
{
	int* p = 0;
	*p = 10;
}

int main(int argc, char* argv[])
{
	//fprintf(stderr, "%s\n", *argv);
	int c; //return value of that option 
	int option_index = 0;
	static struct option long_options[] =
	{
		{"input", required_argument, 0, 'i'},
		{"output", required_argument, 0, 'o'},
		{"segfault", no_argument, 0, 's'},
		{"catch", no_argument, 0, 'c'},
		{"dump-core", no_argument, 0, 'd'},
		{0, 0, 0, 0}
	};

	while(1)
	{
		c = getopt_long(argc, argv, "i:o:scd", long_options, &option_index);
		if (c == -1)
			break;

		if(c == '?')
		{
			fprintf(stderr, "usage: lab0 [--input=FILE|--output=FILE|--dump-core|--segfault|--catch]\n");
			exit(1);
		}
	}

	//looping through to check syntax first
	optind = 0;
	while(1)
	{
		c = getopt_long(argc, argv, "i:o:scd", long_options, &option_index);
		
		if (c == -1)
			break;

		switch(c)
		{
			case 'i':
				if(chin(optarg)) exit(2);
				break;
			case 'o': 
				if(chout(optarg)) exit(3);
				break;
			case 's':
				segfault();
				break;
			case 'c':
				signal(SIGSEGV, catcher);
				break;
			case 'd':
				signal(SIGSEGV, SIG_DFL);
				break;
			case '?':
			default:
				fprintf(stderr,  "BAD: getopt giving nonsensical answer");
				exit(5);
		}
	}


	//action: write from fd 0 to 1
	char buffer[BUFF_SIZE];
	while(1)
	{
		int ammount = read(0, buffer, BUFF_SIZE);
		if(ammount < 0)
		{
			fprintf(stderr, "BAD: read error");
			exit(5);
		}
		else if(ammount == 0)
			break;

		if(!write (1, buffer, ammount))
		{
			fprintf(stderr, "BAD: write error");
			exit(5);
		}
	}

	return 0;
}
