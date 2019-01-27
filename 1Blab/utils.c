#include "utils.h"
#include <stdlib.h> //atoi
#include <string.h> //strlen
#include <ctype.h> //isdigit
#include <stdio.h> //fprintf

//check if string is nonnegative int, then convert
//-1 if not int
int stoi(char* string)
{
	int i;
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

int max(int a, int b)
{
	return a > b ? a : b;
}
