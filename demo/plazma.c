#include <libmega.h>

#include "main.h"
#include "../mega/plazma_pal.h"
#include "../mega/plazma_pat.h"
#include "../mega/plazma_scroll.h"

int go_plazma;
int flip_flop;

void plazma_bottom(void) {
 if(go_plazma == 1) {
  if(flip_flop == 0)
   do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)buffer, 0xe000, 0x1000);
  else
   do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)buffer2,0xe000, 0x1000);
  go_plazma = 0;
 }
}

void init_plazma(void) {
 int i;
 current_pal = plazma_color;
 current_pal_count = 32;

 go_plazma = 0;
 flip_flop =0;

 for(i=0;i<0xe00;i++) {
  VRAM_WRITE(0xc000+i,0);
  VRAM_WRITE(0xe000+i,0);
  buffer[i] = 0;
  buffer2[i] = 0;
 }



 for(i=0;i<3072;i++) {
  VRAM_WRITE(SPRITE_BASE+i,0);
 }

 for(i = 0;i<0x3ff;i++) {
  VRAM_WRITE((0xfc00+i), 0);
 }
 vram_write(0,plazma_pat, 0x4700);
 vram_write(0xc000, plazma_scroll, 0x1000);
 v_hook = plazma_bottom;
 fade_to = 0;
 
}

#include "trig.h"

int xp1=0,xp2=0,yp1=0,yp2=0;

void buildframe(int xp1i, int xp2i, int yp1i, int yp2i) {
 int x,y;
 u16 what;
 u8 val;

 unsigned int xp1f = xp1i, xp2f=xp2i, yp1f=yp1i, yp2f=yp2i;
 unsigned char t1, t2;

 for(y=0;y<28;y++) {
  yp1f++;
  yp2f+=2;
  if(yp1f>255) yp1f -= 255;
  if(yp2f>255) yp2f -= 255;
  xp1f = xp1;
  xp2f = xp2;
  t1 = trig[yp1f];
  t2 = trig[yp2f];
  for(x=0;x<40;x++) {
   xp1f+=2;
   xp2f+=3;
   if(xp1f>255) xp1f -= 255;
   if(xp2f>255) xp2f -= 255;
   val = trig[xp1f]+trig[xp2f]+t1+t2;
   what = (0x221+((val)&0xf)) |0x2000;
   if(flip_flop == 1) {
    buffer[((x+(y*64))<<1)+1] = what&0xff;
    buffer[((x+(y*64))<<1)]=(what>>8)&0xff;
   } else {
    buffer2[((x+(y*64))<<1)+1] = what&0xff;
    buffer2[((x+(y*64))<<1)]=(what>>8)&0xff;
   }

  }
 }
}

void tick_plazma(void) {
 int x, y;

 xp1 -= 3;
 xp2 = (xp2 + 2)%256;
 yp1 -= 3;
 yp2 = (yp2 + 1)%256;
 if(xp1<0) xp1+=256;
 if(yp1<0) yp1+=256;

 buildframe(xp1,xp2,yp1,yp2);

 go_plazma = 1;
 flip_flop^=1;
}

