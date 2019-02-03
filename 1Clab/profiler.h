#ifndef PROFILER_H
#define PROFILER_H

#include <sys/resource.h>
struct rusage usage;
struct timeval u_start, s_start;

void profiler_start();
void profiler_end(char* name);

#endif 
