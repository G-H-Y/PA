#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
#include <stdio.h>

int main() {
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("NDL_Bitmap = %d!\n",sizeof(NDL_Bitmap));
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  printf("in main:pixels\n");
  assert(bmp->pixels);
  printf("after assert\n");
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("after OPENDISP\n");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("after DR\n");
  NDL_Render();
  printf("after render\n");
  NDL_CloseDisplay();
  printf("after close\n");
  while (1);
  return 0;
}
