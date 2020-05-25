#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H
#define ARGS 100
#include "threads/thread.h"


tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

struct archivos{
	struct file* ptr;
	int fd;
	struct list_elem elem;
};

#endif /* userprog/process.h */