#ifndef COMMANDER_H
#define COMMANDER_H

extern int curr_fd;
extern int* curr_fds;
extern int verbose_flag;

typedef struct Command {
	int fds[3];
	char** cmd;
	int args;
} Command;

int command_parse(int argc, char* argv[], Command* answer);
int command_do(Command* gotten);
int command_list(Command* gotten);
int wait();

#endif