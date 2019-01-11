#include <stdio.h>
#include <assert.h>

int main() {
 // printf("in text main\n");
  FILE *fp = fopen("/share/texts/num", "r+");
  
  assert(fp);
 // printf("assert(fp)\n");

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);
 /// printf("assert(size == 5000)\n");

  fseek(fp, 500 * 5, SEEK_SET);
  int i, n;
 // printf("begin first for\n");
  for (i = 500; i < 1000; i ++) {
   // printf("begin scanf\n");
    fscanf(fp, "%d", &n);
    printf("i = %d,n = %d\n",i,n);
    assert(n == i + 1);
  }
  printf("end first for\n");

  fseek(fp, 0, SEEK_SET);
  printf("begin second for\n");
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
