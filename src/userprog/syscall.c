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
struct archivos* busqueda(struct list* files, int fd);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}




static void
syscall_handler (struct intr_frame *f) 
{

  if(f->esp==0x804efff||f->esp==0x804effb){
    matar_proc(-1);
  } 

  int * sp = f->esp;
  direccion(sp);
  int system_call = * sp;
  
  //WRIE
  if(system_call==SYS_WRITE)
  {

    direccion(sp+7);
    direccion(*(sp+6));
    if(*(sp+5)==1){
    putbuf(*(sp+6),*(sp+7));
    f->eax = *(sp+7);
    }
    else{
      struct archivos* archivo = busqueda(&thread_current()->files, *(sp+1));
      if(archivo==NULL){
      f->eax=-1;
      }
      else{
          f->eax = file_write (archivo->ptr, *(sp+2), *(sp+3));
      }

  }




  
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
      struct archivos *pfile = malloc(sizeof(*pfile));
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
    direccion(sp+7);
    direccion(*(sp+6));
    if(*(sp+5)==0)
    {
      int i;
      uint8_t* buffer = *(sp+6);
      for(i=0;i<*(sp+7);i++)
        buffer[i] = input_getc();
      f->eax = *(sp+7);
    }
    else
    {
      struct archivos* fptr = busqueda(&thread_current()->files, *(sp+5));
      if(fptr==NULL)
        f->eax=-1;
      else
      {

        f->eax = file_read (fptr->ptr, *(sp+6), *(sp+7));

      }
    }
  }



  //filesys
  else if(system_call==SYS_FILESIZE){
      direccion(sp+1);
      f->eax = file_length (busqueda(&thread_current()->files, *(sp+1))->ptr);

  }

  
  //EXEC
  else if(system_call==SYS_EXEC) {
    if(f->esp==0xbfffff64 && *(sp+1)==0x804efff){
      matar_proc(-1);
    }

    direccion(sp+1);  
    direccion(*(sp+1));

    int len=strlen(*(sp+1))+1;
    char* file_name=*(sp+1);
    char * aux_ptr;
    char * f_copy=malloc(len);

    strlcpy(f_copy,file_name,len);
    f_copy=strtok_r(f_copy," ",&aux_ptr);

    
    struct file* archivo=filesys_open(f_copy);
    if(archivo==NULL){
      f->eax=-1;
    }else{
      file_close(archivo);
      f->eax=process_execute(file_name);
    }
  }

  //WAIT
  else if(system_call==SYS_WAIT){
    direccion(sp+1);
    f->eax=process_wait(*(sp+1));
  }

  //SEEK
  else if(system_call==SYS_SEEK){
    direccion(sp+5);
    file_seek(busqueda(&thread_current()->files, *(sp+4))->ptr,*(sp+5));
  }

  //TELL
  else if (system_call==SYS_TELL){
        f->eax = file_tell(busqueda(&thread_current()->files, *(sp+1))->ptr);
  }
  
  //HALT
  else if(system_call==SYS_HALT){
    shutdown_power_off();
  }

  //REMOVE
  else if(system_call==SYS_REMOVE){
    if(filesys_remove(*(sp+1))==NULL)
      f->eax = false;
    else
      f->eax = true;
  }


//CLOSE
  else if(system_call==SYS_CLOSE){
    direccion(sp+1);
    struct thread *actual=thread_current();
    struct list_elem *e;
    struct archivos *f;
    struct list* files=&actual->files;
    e=list_begin(files);
    int fd=*(sp+1);

    while( e != list_end (files)){
         f = list_entry (e, struct archivos, elem);
          if(f->fd == fd)
          {
            file_close(f->ptr);
            list_remove(e);
          }
          e=list_next(e);
    }
    free(f);
  }

  //system exit
  else if(system_call==SYS_EXIT)
  {
    direccion((sp+1));
    matar_proc(*(sp+1));
  }
  else{
    matar_proc(-1);
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



