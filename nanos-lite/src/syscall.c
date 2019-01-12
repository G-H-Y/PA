#include "common.h"
#include "syscall.h"
#include "fs.h"

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

  Log("sys = %d", a[0]);
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
   /* if ((fd == 1) || (fd == 2))
    {
      size_t i = 0;
      for (; i < count; i++)
      {
        _putc(buf[i]);
      }
      c->GPRx = count;
    }
    else if (fd > 2)
    {*/
      c->GPRx = fs_write(fd, buf, count);
   // }
    break;
  case SYS_brk:
    //Log("in brk");
    c->GPRx = 0;
    break;
  case SYS_open:
    /*filename = (char*)a[1];
    flags = a[2];
    mode = a[3];*/
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