#include <libmega.h>

#include "main.h"

#include "../mega/vec_pal.h"
#include "../mega/vec_pat.h"
#include "../mega/vec_scroll.h"
#include "../mega/vec_scrollb.h"

int off_x, target;
int looper;

void prescrip_v(void) {
 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);
}

void init_prescrip(void) {
 int i;
 int x, y, sx, sy;

 vram_write(0, vec_pat, 0x3200);
 vram_write(0xc000, vec_scroll, 0x1000);

 for(y=0;y<8;y++)
  for(x=0;x<8;x++) {
   for(sy = 0;sy<4;sy++)
    for(sx = 0;sx<8;sx++) {
     VRAM_WRITE(0xe000 + (((64*(y+(sy*8)))+(x+(sx*8)))*2), 
       vec_scrollb[(((64*y)+x)*2)+1] |(vec_scrollb[(((64*y)+x)*2)]<<8)  );

    }
 
  }



 current_pal = vec_color;
 current_pal_count = 16;

 for(i=0;i<0x3ff;i++)
  VRAM_WRITE(0xfc00+i, 0);
 for(i=0;i<80*8;i++) 
  VRAM_WRITE(SPRITE_BASE+i, 0);

 VSRAM_WRITE(0, 0); 

 dma_corrupt = 0;
 target = 0;
 off_x = 0xfe;
 v_hook = prescrip_v;
 fade_to = 0;
}

void tick_prescrip(void) {
 int i;
 for(i=0;i<244;i++) {
  hscroll_buf[i*2] = off_x;
  hscroll_buf[(i*2)+1] =
   ((cos_tab[((i>>1)+looper)&0xff]*float2fix(80.0f))>>8)>>8;

 }
 if(brightness == 0)
  off_x += ((target-off_x)/8);
 
 looper++;
}
