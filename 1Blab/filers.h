#ifndef FILERS_H
#define FILERS_H

extern int file_flags;
extern int curr_fd;
extern int* curr_fds; 
extern int exit_status;

void new_open(char file[], int flag);
void piper();
void fd_print();

#endif