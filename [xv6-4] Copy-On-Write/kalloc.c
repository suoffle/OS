// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
uint num_free_pages;	//free page 개수
uint pgrefcount[PHYSTOP>>PTXSHIFT]; //물리 메모리[i] 참조 횟수(page number reference count)

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
} kmem;

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
  num_free_pages=0;	//num_free_page initialize
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
    pgrefcount[V2P(p)>>PTXSHIFT]=0;	//page reference counter initialize
    
    kfree(p);
  }

}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)	//v는 물리 메모리 페이지의 가상 주소
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  //memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
    
  if(get_refcount(V2P(v))>0){
  	dec_refcount(V2P((v)));	//decrease physical memory page count  
   } 
  if(get_refcount(V2P(v))==0){
  	memset(v, 1, PGSIZE);
  	r=(struct run*)v;
  	r->next=kmem.freelist;
  	kmem.freelist=r;
  	num_free_pages++;
  }
  
  //r = (struct run*)v;
  //r->next = kmem.freelist;
  //kmem.freelist = r;
  
  //num_free_pages++;	//free page number++
  
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;	//free list pointer

  if(kmem.use_lock)
    acquire(&kmem.lock);
  
  if(num_free_pages>0)
  	num_free_pages--; 
  
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    pgrefcount[V2P(r)>>PTXSHIFT]=1; 
    }
  
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
  
}

//get page reference counter
uint
get_refcount(uint pa)	//physical address
{
	return pgrefcount[pa>>PTXSHIFT];
}

//increase page reference counter
void
inc_refcount(uint pa)	//physical address
{
	pgrefcount[pa>>PTXSHIFT]++;
}

//decrease page reference counter
void
dec_refcount(uint pa)	//physical address
{
	pgrefcount[pa>>PTXSHIFT]--;
}
