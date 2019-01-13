#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"
#include <stdio.h>

#if defined(__ISA_X86__)
intptr_t _syscall_(int type, intptr_t a0, intptr_t a1, intptr_t a2){
  int ret = -1;
  asm volatile("int $0x80": "=a"(ret): "a"(type), "b"(a0), "c"(a1), "d"(a2));
  return ret;
}
#elif defined(__ISA_AM_NATIVE__)
intptr_t _syscall_(int type, intptr_t a0, intptr_t a1, intptr_t a2){
  intptr_t ret = 0;
  asm volatile("call *0x100000": "=a"(ret): "a"(type), "S"(a0), "d"(a1), "c"(a2));
  return ret;
}
#else
#error _syscall_ is not implemented
#endif
int _execve(const char *fname, char * const argv[], char *const envp[]);

void _exit(int status) {
  _syscall_(SYS_exit, status, 0, 0);
  //_execve("/bin/init",NULL,NULL);
  while (1);
}

int _open(const char *path, int flags, mode_t mode) {
  //_exit(SYS_open);
  int ret = _syscall_(SYS_open, (uintptr_t)path, (uintptr_t)flags, (uintptr_t)mode);
  return ret;
}

int _write(int fd, void *buf, size_t count){
  //_exit(SYS_write);
  int ret = _syscall_(SYS_write, (uintptr_t)fd, (uintptr_t)buf, (uintptr_t)count);
  return ret;
}

extern char _end;
intptr_t p_break = (intptr_t)&_end;
void *_sbrk(intptr_t increment){
  intptr_t old_pbreak = p_break;
  int ret = _syscall_(SYS_brk,old_pbreak + increment,0,0);
  if(ret == 0) {
    p_break += increment;
    return (void*)old_pbreak;
  }
  return (void *)-1;
}

int _read(int fd, void *buf, size_t count) {
  //_exit(SYS_read);
  //return 0;
  int ret = _syscall_(SYS_read, (uintptr_t)fd, (uintptr_t)buf, (uintptr_t)count);
  return ret;
}

int _close(int fd) {
  //_exit(SYS_close);
  //return 0;
  int ret = _syscall_(SYS_close, (uintptr_t)fd, 0,0);
  return ret;
}

off_t _lseek(int fd, off_t offset, int whence) {
 // _exit(SYS_lseek);
  //return 0;
  off_t ret = _syscall_(SYS_lseek,(uintptr_t)fd, (uintptr_t)offset, (uintptr_t)whence);
  return ret;
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  //_exit(SYS_execve);
  int ret = _syscall_(SYS_execve,(uintptr_t)fname,(uintptr_t)argv,(uintptr_t)envp);
  return ret;
}

// The code below is not used by Nanos-lite.
// But to pass linking, they are defined as dummy functions

int _fstat(int fd, struct stat *buf) {
  return 0;
}

int _kill(int pid, int sig) {
  _exit(-SYS_kill);
  return -1;
}

pid_t _getpid() {
  _exit(-SYS_getpid);
  return 1;
}
