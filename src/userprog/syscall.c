#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "list.h"
#include "process.h"



static void syscall_handler (struct intr_frame *);
void* check_addr(const void*);
struct proc_file* list_search(struct list* files, int fd);

struct proc_file {
	struct file* ptr;
	int fd;
	struct list_elem elem;
};

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}




static void
syscall_handler (struct intr_frame *f) 
{

  int * p = f->esp;
  int system_call=*p;
  putbuf(*(p+2),*(p+3));
  f->eax = *(p+3);
 
  
}




	
// }


// void* check_addr(const *dir){
// 	return (is_user_vaddr(dir))
// }