#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "list.h"
#include "process.h"




static void syscall_handler (struct intr_frame *);
void* direccion(const void*);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}




static void
syscall_handler (struct intr_frame *f) 
{
  int * sp = f->esp;

  direccion(sp);
  
  int system_call=*sp;

  
 

  //write
  if(system_call==SYS_WRITE)
  {
    direccion(sp+3);
    direccion(*(sp+2));
    direccion(sp+1);
    putbuf(*(sp+2),*(sp+3));
    f->eax = *(sp+3);
  }

  //system exit
  else if(system_call==SYS_EXIT)
  {
    direccion((sp+1));
    matar_proc(*(sp+1));
  }


}


void * direccion(const void *sp){

  if (!is_user_vaddr(sp))
  {
    printf("entreo 1\n");
    matar_proc(-1);
    return 0;
  }

  void *ptr = pagedir_get_page(thread_current()->pagedir,sp);
  
  if (!ptr)
  {
        printf("entreo 2\n");

    matar_proc(-1);
    return 0;
  }
  return ptr;
}



void matar_proc(int estatus){

    struct list_elem *e;
  
      for (e = list_begin (&thread_current()->padre->lista_proc_hijos); e != list_end (&thread_current()->padre->lista_proc_hijos);
           e = list_next (e))
        {
          struct hijo *h = list_entry (e, struct hijo, elem);
          if(h->tid == thread_current()->tid)
          {

            h->used = true;
            h->exit_error = estatus;
          }
        }


  thread_current()->exit_error = estatus;

  if(thread_current()->padre->espera == thread_current()->tid)
    sema_up(&thread_current()->padre->lock_hijo);

  thread_exit();
}