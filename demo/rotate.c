#include <libmega.h>

#include "main.h"
#include "sprites.h"

#include "../mega/logo_pat.h"
#include "../mega/logo_pal.h"
#include "../mega/logo_scrollb.h"

int add_map[8];
s32 rotate_seed;
int looper;

void v_bottom_rotate(void) {
 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);
}

void init_rotate(void) {
 int x,y,sx,sy;
 int i;
 u16 *w;
 current_pal = logo_color;
 current_pal_count = 32;

 vram_write(0, logo_pat, 0x400);

 w = (u16 *)logo_scrollb;
 for(y=0;y<4;y++)
  for(x=0;x<8;x++) {
   for(sy = 0;sy<8;sy++)
    for(sx = 0;sx<8;sx++) {
     VRAM_WRITE(0xe000 + (((64*(y+(sy*4)))+(x+(sx*8)))*2), w[(64*y)+x]);

    }
 
  }
 rotate_seed = 0x15;
 for(i=0;i<8;i++) {
  add_map[i] = rand_short(&rotate_seed)&0x3;
  if(add_map[i] == 0)
   add_map[i] = 1;
 }

 v_hook = v_bottom_rotate; 
 looper = 0;
 fade_to  = 0;
}

void tick_rotate(void) {
 int i;
 for(i=0;i<224;i++) 
  hscroll_buf[(i*2)+1]-=add_map[i/32];
 
 looper++; 
}

