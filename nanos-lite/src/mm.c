#include "memory.h"
#include "am.h"
#include "x86.h"
#include "proc.h"

static void *pf = NULL;
extern PCB *current;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
  printf("in mmbrk\n");
  //need a new page?
  printf("newbrk = %d, maxbrk = %d\n",new_brk,current->max_brk);
  uintptr_t aval_addr = 0xfff - (current->max_brk & 0xfff);
  uintptr_t inc = new_brk - current->max_brk ;
  if(inc > aval_addr){
    //need a new page
    uintptr_t nr_page = inc / PGSIZE;
    if((inc - aval_addr) % PGSIZE) nr_page++;
    void* va = (void*) ((current->max_brk | 0xfff) + 1);
    printf("nrpage = %d\n",nr_page);
    while(nr_page--){
      PDE *pa = (PDE*)(new_page(1));
      _map(&(current->as),va,(void*)pa,1);
      va += PGSIZE;
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %d", pf);

  _vme_init(new_page, free_page);
}
