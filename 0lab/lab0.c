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
#define BUFF_SIZE 100 

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
		fprintf(stderr,  "chin: failed to open %s\n", file);
		return 1;
	}
}

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
		fprintf(stderr,  "chout: failed to create %s\n", file);
		return 1;
	}
}

int main(int argc, char* argv[])
{
	int c; //return value of that option 
	int dump = 1; //whether to dump core or not

	//looping through to check syntax first
	while(1)
	{
		static struct option long_options[] =
		{
			{"input", required_argument, 0, 'i'},
			{"outputY", required_argument, 0, 'o'},
			{"segfault", no_argument, 0, 's'},
			{"catch", no_argument, 0, 'c'},
			{"dump-core", no_argument, 0, 'd'},
			{0, 0, 0, 0}
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "i:o:scd", long_options, &option_index);
		//fprintf(stderr, "%c\n", c);
		
		if (c == -1)
			break;

		switch(c)
		{
			case 'i':
				if(chin(optarg)) exit(1);
				break;
			case 'o': 
				if(chout(optarg)) exit(2);
				break;
			case 's':
				fprintf(stderr, "segfault");
				break;
			case 'c':
				fprintf(stderr, "catching....");
				break;
			case 'd':
				fprintf(stderr ,"dumping-core");
				break;
			case '?':
				fprintf(stderr, "usage: lab0 [--input=FILE|--output=FILE|--dump-core|--segfault|--catch]\n");
				exit(3);
			default:
				fprintf(stderr,  "don't know what's going on");
				exit(5);
		}
	}
	
	//action: write from fd 0 to 1
	int count = 0; //how many bytes were written
	char buffer[BUFF_SIZE];
	while(1)
	{
		int ammount = read(0, buffer, BUFF_SIZE);
		if(ammount < 0)
		{
			fprintf(stderr, "read error");
			exit(5);
		}
		else if(ammount == 0)
			break;

		if(!write (1, buffer, ammount))
		{
			fprintf(stderr, "write error");
			exit(5);
		}
	}
		


	//int size = write(1, "enhey", 3);
	//size += write(1, "oh ho", 3);
	//fprintf(stderr, "%d bytes written to stdout\n", size);


	return 0;

}
