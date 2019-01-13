#include "proc.h"
#include "fs.h"

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  Log("in init_proc");
  //naive_uload(NULL, "/bin/init");
  context_uload(&pcb[0], "/bin/dummy");
  //Log("uload end");
//  context_kload(&pcb[0],(void*)hello_fun);
Log("pcb[0]->as->ptr = %d",pcb[0].as.ptr);
  
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  current->cp = prev;
 // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  current = &pcb[0];
  return current->cp;
}
