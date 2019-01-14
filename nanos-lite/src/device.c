#include "common.h"
#include <amdev.h>
#include <fs.h>

void switch_front(int key);

size_t serial_write(const void *buf, size_t offset, size_t len) {
  //Log("in serial write");
 // _yield();
  int i = 0;
  char *tmp = (char*)buf;
  for(;i < len; i++)
  {
    _putc(tmp[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  //return 0;
 // Log("event!");
  int key = read_key();
  if(key){
    if(key & 0x8000){
      sprintf(buf,"%s %s %c","kd ",keyname[key & 0x7fff],'\n');
      switch_front(key & 0x7fff);
    }
    else{
      sprintf(buf,"%s %s %c","ku ",keyname[key],'\n');
    }
  }
  else{
    sprintf(buf,"%s %d %c","t ",uptime(),'\n');
  }
  int write_len = strlen(buf);
  return (len > write_len) ? write_len : len;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  memcpy(dispinfo+offset,buf,len);
  return len;
  //return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //return 0;
  //Log("len = %d",len);
  int x = (offset / 4) % screen_width();
  int y = (offset / 4) / screen_width();
  int w = len / 4;
  int h = 1;
  //Log("x = %d,y = %d",x,y);
  draw_rect((unsigned int*)buf,x,y,w,h);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  int fd = fs_open("/proc/dispinfo",0,0);
  fs_read(fd,dispinfo,0);
  fs_close(fd);
}
