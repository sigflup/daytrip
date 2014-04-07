#include <stdio.h>

#define 	float2fix(a)	((s16)((a)*256.0f))
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

u16 scrollTable[24*130*sizeof(u16)];

int main(void) {
 int k, h,q;
 int center = 34;
 for (k=0 ; k<130 ; ++k) {
  int count = (k*k)*float2fix(0.0002f);
  count>>=8;
  for (h=0 ; h<20 ; ++h) {
   int xv;
   if (h < 10)
    q = h-2;
   if(h>10)
    q = h+3;
   if (k<center) 
    xv = ((q-10)*(q-10)/17)*((center-k)*(center-k));
   else
    xv = (-(q-10)*(q-10)/17)*((k-center)*(k-center));
   xv *= float2fix(0.004f);
   xv>>=8;
   scrollTable[k*20+h] = 0x840-count+xv;
  }
 }

 printf("const u16 scrollTable[] = { \n");
 for(k = 0;k<20*130;k++) {
  printf("0x%04x,\n", scrollTable[k]);
 }
 printf("};\n");
}
