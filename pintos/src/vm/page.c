#include "vm/page.h"
#include <list.h>
#include <string.h>
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "vm/frame.h"

struct sup_pagetable *sup_pagetable_create (void)
{
  struct sup_pagetable *pt = malloc (sizeof (struct sup_pagetable *));
  if (pt == NULL)
    return NULL;

  list_init (&pt->list_spt);
  return pt;
}


void sup_pagetable_free (struct sup_pagetable *pt)
{
  struct list_elem *e;
  for (e = list_begin (&pt->list_spt); e != list_end (&pt->list_spt);)
    {
      struct sup_pagetable_entry *pte = list_entry (e, struct sup_pagetable_entry, elem); 
      e = list_next (e);

      if (pte == NULL)
        return;

      if (pte->kpage != NULL)
        FT_remove (pte->kpage);
      
      list_remove (&pte->elem);
      free (pte);
    }

  free (pt);
}


bool sup_pagetable_zero (void *user_vp)
{
  struct sup_pagetable_entry *pte = malloc (sizeof (struct sup_pagetable_entry));

  if (pte == NULL)
    return false;

  pte->type = PAGE_ZERO;
  pte->user_vp = user_vp;
  pte->kpage = NULL;
  pte->dirty = false;

  struct sup_pagetable *pt = thread_current()->sup_pagetable;
  list_push_back (&pt->list_spt, &pte->elem);

  return true;
}
