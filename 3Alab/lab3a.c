#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ext_fs.h"
#define BUFF_SIZE 1024

int main(int argc, char* argv[])
{
	int fd; //represents the file being read
	ssize_t reads;
	char block[BUFF_SIZE]; //to read in things

	//invaild arguments?
	if(argc != 2) {
		fprintf(stderr, "usage: lab3a pathname\n");
		exit(1);
	}
	
	//open failed?
	fd = open(argv[1], 0);
	if(fd < 0) {
		fprintf(stderr, "open failed\n");
		exit(1);
	}

	int status;
	struct stat fs;
	status = fstat(fd, &fs);
	if(status < 0) {
		fprintf(stderr, "fstat failed\e");
		exit(1);
	}

	int inodes = fs.st_ino;
	fprintf(stderr, "inode: %d\n", inodes);

	fprintf(stderr, "before: %s\n", block);
	reads = read(fd, block, BUFF_SIZE);
	if(reads < 0) {
		fprintf(stderr, "read failed\n");
		exit (1);
	}
	fprintf(stderr, "reads %zd\n%s\n", reads, block);

	return 0;
}
