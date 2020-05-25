#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <list.h>
#include "filesys/file.h"


enum page_type
  {
    PAGE_ZERO,
    PAGE_FILE,
    PAGE_SWAP
  };


struct sup_pagetable
  {
    struct list list_spt;
  };

struct sup_pagetable_entry
  {
    enum page_type type;
    void *user_vp, *kpage;
    bool dirty;

    union
      {
        struct
          {
            struct file *file;
            off_t offset_pt;
            uint32_t read_bytes;
            uint32_t zero_bytes;
            bool writeable;
          };

        struct
          {
            unsigned swap_index;
          };

      };

    struct list_elem elem;

  };

struct sup_pagetable *sup_pagetable_create (void);
void sup_pagetable_free (struct sup_pagetable *);
bool sup_pagetable_zero (void *user_vp);

#endif