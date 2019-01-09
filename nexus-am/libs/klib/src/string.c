#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  if(s == NULL) 
    return 0;
  size_t cnt = 0;
  size_t i = 0;
  while(s[i]!='\0'){
    cnt++;
    i++;
  }
  return cnt;
}

char *strcpy(char* dst,const char* src) {
  if((dst == NULL) || (src == NULL))
    return NULL;
  size_t i = 0;
  while(src[i]!='\0'){
    dst[i] = src[i];
    i++;
  }
  dst[i]='\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  if((dst == NULL) || (src == NULL) || (n < 0))
    return NULL;
  uint32_t i = 0;
  while((src[i] != '\0') && (i < n)){
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  if((dst == NULL) || (src == NULL))
    return NULL;
  size_t i = 0;
  size_t j = 0;
  while(dst[i] != '\0'){
    i++;
  }
  while(src[j] != '\0'){
    dst[i++] = src[j++];
  }
  dst[i]='\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  if((s1 == NULL) || (s2 == NULL))
    return 0;
  size_t i = 0;
  while((s1[i] != '\0')&&(s2[i] != '\0')){
    if(s1[i] < s2[i]) return -1;
    if(s1[i] > s2[i]) return 1;
    i++;
  }
  if(strlen(s1) < strlen(s2)) return -1;
  if(strlen(s1) > strlen(s2)) return 1;
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  if((s1 == NULL) || (s2 == NULL) || (n < 0))
    return 0;
  size_t i = 0;
  while((i < n) && (s1[i] != '\0') && (s2[i] != '\0')){
    if(s1[i] < s2[i]) return -1;
    if(s1[i] > s2[i]) return 1;
    i++;
  }
  if(i<n){
    if(strlen(s1) < strlen(s2)) return -1;
    if(strlen(s1) > strlen(s2)) return 1;
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  if((v == NULL) || (n < 0))
    return NULL;
  char* tmpdst = (char*)v;
  char tmpval = (char)c;
    while(n--)
    {
        *tmpdst++ = tmpval;
    }
    return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  if((out == NULL) || (in == NULL)|| (n < 0))
    return NULL;
  printf("in memcpy\n");
  char* tmpout = (char*)out; 
  printf("char* tmpout = (char*)out\n");
  char* tmpin = (char*)in;
  printf("char* tmpin = (char*)in\n");
  size_t i = 0;
  for(;(i < n) && (tmpin[i] != '\0'); i++){
    tmpout[i] = tmpin[i];
    printf("i = %d\n",i);
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if((s1 == NULL) || (s2 == NULL)|| (n < 0))
    return 0;
  char* tmps1 = (char*)s1;
  char* tmps2 = (char*)s2;
  size_t i = 0;
  while((i < n) && (tmps1[i] != '\0') && (tmps2[i] != '\0')){
    if(tmps1[i] < tmps2[i]) return -1;
    if(tmps1[i] > tmps2[i]) return 1;
    i++;
  }
  if(i<n){
    if(strlen(tmps1) < strlen(tmps2)) return -1;
    if(strlen(tmps1) > strlen(tmps2)) return 1;
  }
  return 0;
}

#endif
