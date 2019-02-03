#ifndef COMMANDER_H
#define COMMANDER_H

extern int curr_fd;
extern int* curr_fds;
extern int num_proc;
typedef struct Command {
	int fds[3];
	char** cmd;
	int args;
	int pid;
} Command;
extern Command* curr_proc;


int command_parse(int argc, char* argv[], Command* answer);
int command_do(Command* gotten);
int command_list(Command* gotten);
void command_free();
int waiter();

#endif
