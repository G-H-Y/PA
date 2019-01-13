#include <x86.h>
#include <klib.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*pgalloc_usr)(size_t);
static void (*pgfree_usr)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  //printf("start init\n");
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }
  printf("start set cr3 = %d\n",(int)kpdirs);
  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  //printf("in vem init: have set cr0\n");
  return 0;
}

int _protect(_Protect *p) {
  printf("in protect\n");
  PDE *updir = (PDE*)(pgalloc_usr(1));
  printf("updir = %d\n",(uint32_t)updir);
  p->pgsize = 4096;
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
  return 0;
}

void _unprotect(_Protect *p) {
}

static _Protect *cur_as = NULL;
void get_cur_as(_Context *c) {
  c->prot = cur_as;
}

void _switch(_Context *c) {
  printf("in switch\n");
  set_cr3(c->prot->ptr);
  cur_as = c->prot;
}

int _map(_Protect *p, void *va, void *pa, int mode) {
  printf("in map: va = %d\n",(uint32_t)va);
  uint32_t pd_offset = ((uintptr_t)va >> 22) << 2;
  uint32_t* ptr = p->ptr;
  printf("in map: ptr = %d\n",(uint32_t)ptr);
  printf("in map: ptr+offset = %d\n",(uint32_t)ptr+pd_offset);
  uint32_t pd_entry = ptr[pd_offset];
  printf("pd_entry = %d\n",pd_entry);
  if(pd_entry == kpdirs[pd_offset]){
    PDE *uptable = (PDE*)(pgalloc_usr(1));
    ptr[pd_offset] = (uintptr_t)uptable | PTE_P;
    pd_entry = ptr[pd_offset];
    printf("in map: ptr[offset] = %d\n",ptr[pd_offset]);
  }
  
  uint32_t pt_offset = (((uintptr_t)va >> 12) &(0x3ff)) << 2;
  uint32_t* pt = (uint32_t*)(pd_entry & 0xfffff000);
  pt[pt_offset] = (uintptr_t)pa | PTE_P;
  printf("/////////////////////////////////\n");
  return 0;
}

_Context *_ucontext(_Protect *p, _Area ustack, _Area kstack, void *entry, void *args) {
   _Context *tf = ustack.end - sizeof(_Context) - 3 * sizeof(uintptr_t);
  tf->eip = (uintptr_t)entry;
  tf->cs = 8;
  tf->prot->ptr = p->ptr;
  int * tmp = ustack.end - 3 * sizeof(uintptr_t);
  int i = 0;
  while(i<3){
    tmp[i] = 0;
    i++;
  }
  //printf("in ucontext: tf = %d\n",tf);
  return tf;
}
