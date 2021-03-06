#include "proc.h"
#include "fs.h"
#include "amdev.h"
#include "klib.h"

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;
PCB *front = NULL;

void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j++;
    _yield();
  }
}

void init_proc()
{
  Log("in init_proc");
  //naive_uload(NULL, "/bin/init");
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  //Log("uload pal end");
  //Log("pcb[0]->as->ptr = %d",pcb[1].as.ptr);

  //Log("pcb[0]->as->ptr = %d",pcb[0].as.ptr);
  //Log("uload hello end");
  switch_boot_pcb();
}

static uint32_t cnt = 0;
_Context *schedule(_Context *prev)
{
  current->cp = prev;
  cnt++;
  // current = &pcb[1];
  if (front == NULL)
  {
    front = &pcb[1];
  }
  current = (current == &pcb[0]) ? front : &pcb[0];
  if ((current == &pcb[0]) && (cnt % 1000))
  {
    current = front;
  }
  //current = (current == &pcb[1]) ? &pcb[0] : &pcb[1];
  /*if(current == &pcb[1]){
   printf("begin pal!\n");
 }
 if(current == &pcb[0])
 {
   printf("begin hello!\n");
 }*/
  //current = &pcb[0];
  return current->cp;
}

void switch_front(int key)
{

  switch (key)
  {
  case _KEY_F1:
    front = &pcb[1];
    break;
  case _KEY_F2:
    front = &pcb[2];
    break;
  case _KEY_F3:
    front = &pcb[3];
    break;
  default:
   // Log("not F");
    break;
  }
}
