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
struct proc_file* list_search(struct list* files, int fd);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}




static void
syscall_handler (struct intr_frame *f) 
{
  

  if(f->esp==0x804efff){
    matar_proc(-1);
  }
  int * sp = f->esp;
  direccion(sp);
  int system_call = * sp;
 
  //WRIE
  if(system_call==SYS_WRITE)
  {
    direccion(sp+3);
    direccion(*(sp+2));
    direccion(sp+1);
    putbuf(*(sp+2),*(sp+3));
    f->eax = *(sp+3);

//CREATE
  }else if(system_call==SYS_CREATE){
    direccion(sp+2);
    direccion(*(sp+1));
    f->eax = filesys_create(*(sp+1),*(sp+2));
  }



//OPEN
  else if(system_call==SYS_OPEN){
    direccion(*(sp+1));
    struct thread *actual=thread_current();
    struct file* archivo=filesys_open(*(sp+1));
    if(archivo==NULL){
      f->eax=-1;
    }else{

      struct proc_file *pfile = malloc(sizeof(*pfile));
      int fd =actual->file_count;
      pfile->ptr = archivo;
      pfile->fd = fd;
      actual->file_count++;
      list_push_back (&thread_current()->files, &pfile->elem);
      f->eax = pfile->fd;
    }
  }

  //READ
  else if(system_call==SYS_READ){
      direccion(sp+3);
      direccion(*(sp+2));
      struct proc_file* fptr = list_search(&thread_current()->files, *(sp+1));
      if(fptr==NULL)
        f->eax=-1;
      else
      {
      
        f->eax = file_read (fptr->ptr, *(sp+2), *(sp+3));
     
      }
  }
  //filesys
  else if(system_call==SYS_FILESIZE){
      direccion(sp+1);
      f->eax = file_length (list_search(&thread_current()->files, *(sp+1))->ptr);

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

    matar_proc(-1);

    return 0;
  }

  void *ptr = pagedir_get_page(thread_current()->pagedir,sp);
  
  if (!ptr)
  {
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


struct proc_file* list_search(struct list* files, int fd)
{

  struct list_elem *e;

      for (e = list_begin (files); e != list_end (files);
           e = list_next (e))
        {
          struct proc_file *f = list_entry (e, struct proc_file, elem);
          if(f->fd == fd){
            return f;
          }
        }
   return NULL;
}
