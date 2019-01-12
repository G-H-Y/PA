#include "proc.h"
#include "fs.h"

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);

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
  //Log("in init_proc");
  //naive_uload(NULL, "/bin/init");
  context_kload(&pcb[0],(void*)hello_fun);
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  //return NULL;
  Log("schedule");
  current->cp = prev;
  current = &pcb[0];
  printf("current->cp = %d\n",current->cp);
  return current->cp;
}
