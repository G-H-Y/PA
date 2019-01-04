#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

uint32_t write_int(char* buffer,int value)
{
    unsigned char stack[100];
    uint32_t length = 0;
    uint32_t tmp = 0;
    if(value<0){
        *(buffer++) = '-';
        value *= -1;
    }
    do
    {
        stack[length] = '0' + (value % 10);
        value /= 10;
        length++;
    }while(value);

    tmp = length;
    while(length)
    {
        *buffer = stack[length-1];
        length--;
        buffer++;
    }
    return tmp;
}

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int snprintf(char *out,size_t n, const char *fmt, ...) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
    va_list arg_ptr;
    //uint32_t out_length = strlen(out);
    uint32_t fmt_length = strlen(fmt);
    uint32_t index = 0;
    uint32_t cnt = 0;
     
    int a_int;
    char a_char;
    char* a_str;
     
    va_start(arg_ptr,fmt);
    for(index = 0; index < fmt_length; index++)
    {
      cnt++;
      if(fmt[index] != '%')
        (*out++) = fmt[index];
      else
      {
        switch(fmt[index+1])
        {
          case 'd':
            a_int = va_arg(arg_ptr,int);
            out += write_int(out,a_int);
            break;
          case 's':
            a_str =(char*)va_arg(arg_ptr,char*);
            strcpy(out,a_str);
            out += strlen(a_str);
            break;
          case 'c':
            a_char = va_arg(arg_ptr,int);
            *(out++) = a_char;
            break;
        }
        index++;
      }
    }
    va_end(arg_ptr);
    return cnt;
}

#endif
