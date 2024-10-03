// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  uint count[PHYSTOP >> PGSHIFT];
} kmem;

int free_frame_cnt = 0; // xv6 proj - cow

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
    kfree(p);
    kmem.count[V2P(p) >> PGSHIFT] = 0;}
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;

  if(kmem.count[V2P(v) >> PGSHIFT] > 0)
    --kmem.count[V2P(v) >> PGSHIFT];

  if(kmem.count[V2P(v) >> PGSHIFT] == 0){
    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);
    free_frame_cnt++; // xv6 proj - cow
    r->next = kmem.freelist;
    kmem.freelist = r;

  }

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
  {
    kmem.freelist = r->next;
	free_frame_cnt--; // xv6 proj - cow
  kmem.count[V2P((char*)r) >> PGSHIFT] = 1;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

void decrementReferenceCount(uint pa)
{ 
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("decrementReferenceCount");

  acquire(&kmem.lock);
  --kmem.count[pa >> PGSHIFT];
  release(&kmem.lock);
}

void incrementReferenceCount(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("incrementReferenceCount");

  acquire(&kmem.lock);
  ++kmem.count[pa >> PGSHIFT];
  release(&kmem.lock);
}
uint getReferenceCount(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("getReferenceCount");
  uint count;

  acquire(&kmem.lock);
  count = kmem.count[pa >> PGSHIFT];
  release(&kmem.lock);

  return count;
}
