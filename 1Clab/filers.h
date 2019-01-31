#ifndef FILERS_H
#define FILERS_H

extern int file_flags;
extern int curr_fd;
extern int* curr_fds; 

int opener(char file[], int flag);
int closer(char* fd);
int piper();
void fd_print();

#endif