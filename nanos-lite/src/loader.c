#include "proc.h"
#include "fs.h"

size_t get_ramdisk_size();
size_t ramdisk_read(void *buf, size_t offset, size_t len);
#define DEFAULT_ENTRY 0x4000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename,0,0);
  if(fd){
    fs_read(fd,(void*)DEFAULT_ENTRY,fs_filesz(fd));
    fs_lseek(fd,0,SEEK_SET);
    fs_close(fd);
  }
  
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
  //Log("begin loader");
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
  //Log("leave uload");
}
