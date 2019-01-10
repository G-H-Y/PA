#include "common.h"
#include "syscall.h"

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

  switch (a[0])
  {
  case SYS_yield:
    _yield();
    c->GPRx = 0;
    break;
  case SYS_exit:
    _halt(a[1]);
    break;
  case SYS_write:
    if ((fd == 1) || (fd == 2))
    {
      size_t i = 0;
      for (; i < count; i++)
        _putc(buf[i]);
    }
    //Log("in sys_write");
    c->GPRx = count;
    break;
 // case SYS_brk:
 //   c->GPRx = 0;
 //   break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}