#include "signals.h"
#include <stdio.h>

void aborter()
{
	int* p = 0;
	*p = 10;
}
