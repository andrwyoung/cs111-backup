#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>

int stoi(char* string)
{
	unsigned int i;
	for(i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
		{
			fprintf(stderr, "not a number\n");
			return -1;
		}
	}
	return atoi(string);
}

int main(int argc, char* argv[]) 
{
	int c; //return value of that option
	int option_index = 0;
	static struct option long_options[] = 
	{
		{"scale",	required_argument,	0, 'k'},
		{"period",	required_argument,	0, 'p'},
		{"log",		required_argument,	0, 'l'},
		{0, 0, 0, 0,}
	};
	
	while(1)
	{
		c = getopt_long(argc, argv, "", long_options, &option_index); 
		if(c == -1)
			break;

		switch(c)
		{
			case 'k':
				if(*optarg != 'F' || *optarg != 'C')
				{
					fprintf(stderr, "SCALE: invalid argument (F or C)\n");
					exit(1);
				}
				break;
			case 'p':
				{
					int num = stoi(optarg);
					if (num < 0)
					{
						fprintf(stderr, "PEROID: argument must be number\n");
						exit(1);
					}
				}
			case 'l':
				fprintf(stderr, "logging? do a funtion?\n");
				break;
			case '?':
				fprintf(stderr, "ERROR: invalid argument\n");
				exit(1);
			default:
				fprintf(stderr, "FATAL\n");
			//mark
		}
	}
	return 0;
}
