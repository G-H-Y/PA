#include "fs.h"

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    {"stdin", 0, 0, 0, invalid_read, invalid_write},
    {"stdout", 0, 0, 0, invalid_read, invalid_write},
    {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *pathname, int flags, int mode)
{
  int fd = 0;
  for (; fd < NR_FILES; fd++)
  {
    if (strcmp(file_table[fd].name, pathname) == 0)
    {
      Log("pathname = %s",pathname);
      break;
    }
  }
  return fd;
}

size_t fs_filesz(int fd)
{
  return file_table[fd].size;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  if (fd>= FD_FB)
  {
    //Log("file size = %d,len = %d",file_table[fd].size,len);
    Log("before read: openoffset = %d",file_table[fd].open_offset);
    size_t disk_offset = file_table[fd].disk_offset;
    size_t open_offset = file_table[fd].open_offset;
    size_t fsize = file_table[fd].size;
    size_t aval_size = fsize - open_offset ;
    //Log("aval_size = %d",aval_size);
    size_t read_len = (aval_size > len) ? len : aval_size;
    //Log("offset = %d,read_len = %d",disk_offset + open_offset,read_len);
    len = ramdisk_read(buf, disk_offset + open_offset, read_len);
    Log("read len = %d",len);
    fs_lseek(fd, open_offset + len, SEEK_SET);
    Log("after read: openoffset = %d",file_table[fd].open_offset);
    return ((open_offset + read_len) == fsize) ? 0 : read_len;
  }
  return -1;
}

__off_t fs_lseek(int fd, __off_t offset, int whence)
{
  if (fd)
  { 
    Log("whence = %d",whence);
    switch (whence)
    {
    case SEEK_SET:
      if (offset <= file_table[fd].size)
      {
        file_table[fd].open_offset = offset;
        return file_table[fd].open_offset;
      }
    case SEEK_CUR:
      if ((offset + file_table[fd].open_offset) <= file_table[fd].size)
      {
        file_table[fd].open_offset += offset;
        return file_table[fd].open_offset;
      }
    case SEEK_END:
      if ((offset + file_table[fd].size) <= file_table[fd].size)
      {
        file_table[fd].open_offset = file_table[fd].size + offset;
        return file_table[fd].open_offset;
      }
    }
    return -1;
  }

  return -1;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  if(fd){
    Log("fd = %d",fd);
    size_t disk_offset = file_table[fd].disk_offset;
    size_t open_offset = file_table[fd].open_offset;
    size_t fsize = file_table[fd].size;
    size_t aval_size = fsize - open_offset;
    size_t write_len = (aval_size > len) ? len : aval_size;
    size_t len = ramdisk_write(buf, disk_offset + open_offset,write_len);
    fs_lseek(fd, open_offset + len, SEEK_SET);
    return write_len;
  }
  return -1;
}

int fs_close(int fd)
{
  Log("close file: %d",fd);
  file_table[fd].size = 0;
  return 0;
}

void init_fs()
{
  // TODO: initialize the size of /dev/fb
}
