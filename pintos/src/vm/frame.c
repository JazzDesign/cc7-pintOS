#include "vm/frame.h"
#include <debug.h>
#include <list.h>
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"

struct frame
{
  void *kpage;
  struct list_elem e1;
};


struct list ft;
struct lock ft_lock;

void FT_init(void)
{
  list_init(&ft);
  lock_init(&ft_lock);
}
  

void FT_evict(void *kpage)
{
  lock_acquire(&ft_lock);

  struct frame *f = NULL;
  struct list_elem *e = list_begin(&ft);

  while(e != list_end(&ft)){
    struct frame *f1 = list_entry (e, struct frame, e1);
    
    if (f1->kpage == kpage)
    {
      f = f1;
      break;
    }

    e = list_next(e);
  }

  if (f != NULL)
    {
      list_remove (&f->e1);
      free (f);
    }

  palloc_free_page (kpage);

  lock_release(&ft_lock);
}


void FT_remove (void *kpage)
{
  lock_acquire(&ft_lock);

  struct frame *f = NULL;
  struct list_elem *e = list_begin(&ft);

  while(e != list_end(&ft)){
    struct frame *f1 = list_entry (e, struct frame, e1);
    
    if (f1->kpage == kpage)
    {
      f = f1;
      break;
    }

    e = list_next(e);
  }

  if (f != NULL)
    {
      list_remove (&f->e1);
      free (f);
    }

  lock_release(&ft_lock); 
}


void *FT_write (enum palloc_flags flags)
{
    ASSERT (flags & PAL_USER);
    lock_acquire (&ft_lock);
    void *kpage = palloc_get_page (flags); 
    
    if (kpage == NULL)
    {
      lock_release (&ft_lock);
      return NULL;
    }

    struct frame *f = malloc (sizeof (struct frame));

    if (f == NULL)
    {
        palloc_free_page(kpage);
        return NULL;
    }

    f->kpage = kpage;
    list_push_back (&ft, &f->e1);

    lock_release (&ft_lock);

    return kpage;
}
