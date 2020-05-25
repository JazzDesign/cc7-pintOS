#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <list.h>
#include "threads/palloc.h"

void FT_init(void);
void FT_evict (void *kpage);
void FT_remove (void *kpage);
void *FT_write (enum palloc_flags);

#endif
