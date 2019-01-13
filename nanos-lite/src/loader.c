#include "proc.h"
#include "fs.h"
#include "x86.h"

size_t get_ramdisk_size();
size_t ramdisk_read(void *buf, size_t offset, size_t len);
int _protect(_Protect *p);
int _map(_Protect *p, void *va, void *pa, int mode);
#define DEFAULT_ENTRY 0x8048000

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename,0,0);
  if(fd){
    size_t file_size = fs_filesz(fd);
    int nr_page = file_size / PGSIZE ;
    if(file_size % PGSIZE) nr_page++;
    uint32_t *pa = NULL;
    uint32_t *va = (uint32_t*)DEFAULT_ENTRY;
    while(nr_page--){
      pa = (uint32_t*)(new_page(1));
      _map(&(pcb->as),(void*)va,(void*)pa,1);
      fs_read(fd,(void*)pa,PGSIZE);     
      va += PGSIZE;
    }   
    fs_lseek(fd,0,SEEK_SET);
    fs_close(fd);
  }
  Log("end load");
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  Log("begin loader");
  _protect(&(pcb->as));
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
  Log("leave uload");
}
