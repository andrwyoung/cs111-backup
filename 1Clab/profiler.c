#include "profiler.h"
#include <stdio.h>
#include <sys/resource.h>

void profiler_start()
{
	int ret = getrusage(RUSAGE_SELF, &usage);
	if(!ret)
	{	
		u_start = usage.ru_utime;
		s_start = usage.ru_stime;
		//printf("user start: %lds %dus ", u_start.tv_sec, u_start.tv_usec);
		//printf("system start: %lds %dus\n", s_start.tv_sec, s_start.tv_usec);
	}
	else 
		fprintf(stderr, "profile start error: %d\n", ret);
}

void profiler_end(char* name)
{
	printf("--%s\t", name);

	int ret = getrusage(RUSAGE_SELF, &usage);
	if(!ret)
	{	
		long u_diff_sec = usage.ru_utime.tv_sec - u_start.tv_sec;
		int u_diff_usec = usage.ru_utime.tv_usec - u_start.tv_usec;	
		long s_diff_sec = usage.ru_stime.tv_sec - s_start.tv_sec;
		int s_diff_usec = usage.ru_stime.tv_usec - s_start.tv_usec;

		//printf("user end: %lds %dus ", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
		//printf("system end: %lds %dus\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
		printf("user: %lds %dus, ", u_diff_sec, u_diff_usec);
		printf("system: %lds %dus\n", s_diff_sec, s_diff_usec);
		fflush(stdout);
	}
	else 
		fprintf(stderr, "profile end error: %d\n", ret);
}
