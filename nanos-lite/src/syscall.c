#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"
extern PCB *current;
void naive_uload(PCB *pcb, const char *filename);
int mm_brk(uintptr_t new_brk);

_Context *do_syscall(_Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  int fd = a[1];
  char *buf = (char *)a[2];
  size_t count = a[3];

  __off_t offset = a[2];
  int whence = a[3];

  char *filename = (char *)a[1];
  int flags = a[2];
  int mode = a[3];

  uintptr_t new_brk = a[1];
  //Log("sys = %d", a[0]);
  switch (a[0])
  {
  case SYS_yield:
    _yield();
    c->GPRx = 0;
    break;
  case SYS_exit:
  //Log("in exit");
    _halt(a[1]);
    break;
  case SYS_execve:
    naive_uload(NULL, filename);
  case SYS_write:
    c->GPRx = fs_write(fd, buf, count);
    break;
  case SYS_brk:
  Log("newbrk = %d,max_brk = %d",new_brk,current->max_brk);
    if (current->max_brk < new_brk){
      Log("newbrk = %d\n",new_brk);
      mm_brk(new_brk);
      current->max_brk = new_brk;
    }
    current->cur_brk = new_brk;
    c->GPRx = 0;
    break;
  case SYS_open:
    c->GPRx = fs_open(filename, flags, mode);
    break;
  case SYS_read:
    c->GPRx = fs_read(fd, buf, count);
    break;
  case SYS_lseek:
    c->GPRx = fs_lseek(fd, offset, whence);
    break;
  case SYS_close:
    c->GPRx = fs_close(fd);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}