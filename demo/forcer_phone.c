#include <libmega.h>

#include "main.h"

#include "../mega/forcer_phone_pal.h"
#include "../mega/forcer_phone_pat.h"
#include "../mega/forcer_phone_scroll.h"
#include "../mega/forcer_phone_scrollb.h"


int r_bar, g_bar, b_bar;

int r_looper, g_looper, b_looper;
int ring_looper;

int line_counter; 
u16 bars_buf[224];
int looper;

void h_bottom_bars(void) {
 CRAM_WRITE(0,bars_buf[line_counter]);
 line_counter++;
}

void v_bottom_bars(void) {
// do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);
 VSRAM_WRITE(2, ring_looper);
 line_counter = 0;
}

void init_forcer_phone(void) {
 int i;
 v_hook = no_routine;
 h_hook = no_routine;

 current_pal = forcer_phone_color;
 current_pal_count = 32;

 vram_write(0,forcer_phone_pat, 0x3900);
 vram_write(0xc000, forcer_phone_scroll, 0x1000);
 vram_write(0xe000, forcer_phone_scrollb, 0x1000);

 for(i=0;i<244*2;i++) 
  VRAM_WRITE(0xfc00+i,0);

 VSRAM_WRITE(0,0);
 VSRAM_WRITE(2,0);

 VDP_SET_REGISTER(H_COUNTER, 1);

 line_counter = 0;
 looper = 0;
 fade_to = 0;
}



void tick_forcer_phone(void) {
 int i, j, k;
 int r,g,b;

 if(((r_looper >> 8) &1)==1)
  ring_looper = r_looper &0xf;
 else
  ring_looper = 0;


 if(brightness ==0) {
  h_hook = h_bottom_bars;
  v_hook = v_bottom_bars;
 }
 r_looper+=5;
 g_looper+=3;
 b_looper+=4;
 r_bar = (((cos_tab[r_looper&0xff]*float2fix(30.0f))>>8)>>8)+50;
 g_bar = (((cos_tab[g_looper&0xff]*float2fix(30.0f))>>8)>>8)+50;
 b_bar = (((cos_tab[b_looper&0xff]*float2fix(30.0f))>>8)>>8)+50;
  
 for(i=0;i<224;i++) {

  k = abs(r_bar-i);
  if(k<8)
   bars_buf[i] = VDP_CRAM_COLOR(7-k,0,0);
  else
   bars_buf[i]=0; 

  k = abs(g_bar-i);
  if(k<8)
   bars_buf[i]|= VDP_CRAM_COLOR(0,7-k,0);

  k = abs(b_bar-i);
  if(k<8)
   bars_buf[i] |= VDP_CRAM_COLOR(0,0,7-k);

  bars_buf[i]^=0xffff;

 }


}
