#include "proc.h"
#include "fs.h"

size_t get_ramdisk_size();
size_t ramdisk_read(void *buf, size_t offset, size_t len);
#define DEFAULT_ENTRY 0x4000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  //printf("in loader\n");
  //printf("ramdisk size = %d\n",get_ramdisk_size());
  //ramdisk_read((void*)DEFAULT_ENTRY,0,get_ramdisk_size());
  Log("filename : %s",filename);
  int fd = fs_open(filename,0,0);
  Log("open file %d",fd);
  if(fd){
    fs_read(fd,(void*)DEFAULT_ENTRY,fs_filesz(fd));
    Log("read file size %d",fs_filesz(fd));
    fs_close(fd);
    Log("close file");
  }
  
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  Log("filename : %s",filename);
  uintptr_t entry = loader(pcb, filename);
  //printf("in native uload: entry = %d\n",entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
