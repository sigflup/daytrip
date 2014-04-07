#include <libmega.h>

#include "main.h"
#include "scene.h"
#include "sprites.h"

#include "../mega/test_pat.h"
#include "../mega/test_pal.h"
#include "../mega/test_scroll.h"
#include "../mega/test_scrollb.h"

#define SPEED	0.6f

int dist_map[128];
int dist_map2[128];


int scanline;

s16 looper;
int looper_dir;
int looper2;

void h_bottom_test(void) {
  SET_VSRAM_ADDRESS(0);
  address = (u16 *)VDP_DATA;
  *address =0;
  *address =-scanline - (scanline-dist_map[scanline]);
/* VSRAM_WRITE(0, -scanline - (scanline-dist_map[scanline]));
 VSRAM_WRITE(4, -scanline - (scanline-dist_map2[scanline])); */

 scanline++;
}

void v_bottom_test(void) {
 scanline = 0;
}

void init_test(void) {
 int i;
 current_pal = test_color;
 current_pal_count = 32;
 VDP_SET_REGISTER(H_COUNTER, 2);
 VDP_SET_INC(2);

 vram_write(0, test_pat, 0x8000);
 vram_write(0xc000, test_scroll, 0x1000);
 vram_write(0xe000, test_scrollb, 0x1000);


 scanline = 0;

 for(i=0;i<0x3ff;i++)
  VRAM_WRITE(0xfc00+i, 0);
 for(i=0;i<80*8;i++) 
  VRAM_WRITE(SPRITE_BASE+i, 0);
 
 SET_VSRAM_ADDRESS(2);
 for(i=0;i<20;i++) {
  *address = 0;
 }

 v_hook = v_bottom_test;
 h_hook = h_bottom_test;
 looper = float2fix(-32.0f);
 looper2 = 0;
 looper_dir = 0;
 fade_to = 0;

 VDP_SET_REGISTER(MODE_SET_3, HORIZONTAL_LINE);

 updating_palette = 1;
}

void tick_test(void) {
 int i;

 for(i=0;i<128;i++) {
  dist_map[i] = (((cos_tab[((i*4)+looper2)&0xff]*looper)>>8)>>8)+64; 
//  dist_map2[i] =dist_map[i]*-1;
//  dist_map2[i] += 64;
//  map2[i] = (((cos_tab[((i*4)+looper2)&0xff]*(looper*-1))>>8)>>8)+64; 
 }

 if(looper_dir == 0) {
  looper+=float2fix(SPEED);
  if(looper>float2fix(32.0f)) {
   looper = float2fix(32.0f);
   looper_dir = 1;
  }
 } else {
  looper-=float2fix(SPEED);
  if(looper<float2fix(-32.0f)) {
   looper = float2fix(-32.0f);
   looper_dir = 0;
  }

 }
 
 looper2+=2;
}
