// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  int ref[1024*32];
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");

  for(int i=0;i<1024*32; i++){
    kmem.ref[i] = 0;
  }

  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  uint64 index;
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.

  r = (struct run*)pa;



  acquire(&kmem.lock);


  index = REF_INDEX((uint64)pa);

  // if((uint64)pa == 0x0000000087f73000){
  //   printf("kfree: pa=[%p], ref=[%d]\n",pa,kmem.ref[index]);
  // }

  if(kmem.ref[index] > 0){
    kmem.ref[index] -= 1;
  }

  if(kmem.ref[index] > 0){
    release(&kmem.lock);
    return;
  }

  kmem.ref[index] = 0;

  memset(pa, 1, PGSIZE);

  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    uint64 index;
    index = REF_INDEX((uint64)r);
    kmem.ref[index] = 1;
    // printf("kalloc: pa=[%p], ref=[%d]\n",r,kmem.ref[index]);
  }

  release(&kmem.lock);

  return (void*)r;
}

void cow_add_ref(uint64 pa){
  acquire(&kmem.lock);
  uint64 index;
  index = REF_INDEX(pa);
  kmem.ref[index] += 1;
  release(&kmem.lock);
}

int cow_ref_num(uint64 pa){
  int result;
  acquire(&kmem.lock);
  uint64 index;
  index = REF_INDEX(pa);
  result = kmem.ref[index];
  release(&kmem.lock);
  return result;
}