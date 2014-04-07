#include <libmega.h>

#include "main.h"

#include "../mega/vertical_pal.h"
#include "../mega/vertical_pat.h"
#include "../mega/vertical_scrollb.h"
#include "../mega/vertical_scrolla.h"

#define SPEED  float2fix(0.04f)
u16 ttt;
u16 *scrollData;
int posY;
int inY;
int posval;

int pq;



#include "vert_tab.h"

void h_bottom_vertical(void) {
 int x;
 SET_VSRAM_ADDRESS(2);
 address = (u16 *)VDP_DATA;

 __asm__(
   "move.l address,%a0		\n"
   "move.l posval,%d3 		\n"
   "move.l scrollData,%d0	\n"
   "move.l %d0, %a1		\n"
   "move.l #2, %d2		\n"
 );

#define DO_VERT \
 __asm__( \
   "move.w (%a1), %d1		\n" \
   "add.w %d3, %d1		\n" \
   "move.w %d1, (%a0)		\n" \
   "add.l %d2, %d0		\n" \
   "move.l %d0,%a1		\n" \
 );

 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;
 DO_VERT;

#undef DO_VERT

 scrollData+=20;
}

void v_bottom_vertical(void) {
 ++ttt;
 posY+=inY;
 if(posY>0) inY-=SPEED;
 if(posY<0) inY+=SPEED;
 posval = -(posY>>8)-(ttt/4);
 scrollData = scrollTable;
}

void init_vertical(void) {
 int x,y,sx,sy;
 int i,j;
 u16 *w;
 current_pal = vertical_color;
 current_pal_count = 32;
 vram_write(0, vertical_pat, 0x6500);

 w = (u16 *)vertical_scrollb;
 for(y=0;y<16;y++)
  for(x=0;x<16;x++) {
   for(sy = 0;sy<2;sy++)
    for(sx = 0;sx<3;sx++) {
      VRAM_WRITE(0xe000 + (((64*(y+(sy*16)))+(x+(sx*16)))*2), w[(64*y)+x]);
    }
 
  }

 vram_write(0xc000, vertical_scrolla, 0x1000);
 VDP_SET_REGISTER(MODE_SET_3, HORIZONTAL_LINE | VERTICAL_2CELL);
 VDP_SET_INC( 4);
 VDP_SET_REGISTER(H_COUNTER, 2);

 ttt = 0;
 posY = float2fix(-40.0f);
 inY = 0;
 scrollData = scrollTable;

 updating_palette = 1;
 dma_corrupt = 1;
 v_hook = v_bottom_vertical;
 h_hook = h_bottom_vertical;
 pq = 0;
 fade_to = 0;
}

void tick_vertical(void) {
/* pq++;
 if(pq > 10) 
  updating_palette = 0; */
}
