#include "utils.h"
#include <stdlib.h> //atoi
#include <string.h> //strlen
#include <ctype.h> //isdigit

//check if string is int, then convert
//-1 if not int
int stoi(char* string)
{
	int i;
	for(i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
			return -1;
	}
	return atoi(string);
}