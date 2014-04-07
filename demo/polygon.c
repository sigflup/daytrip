#include <libmega.h>

#include "main.h"
#include "scene.h"
#include "sprites.h"
#include "vert.h"

#include "../mega/debris_pal.h"
#include "../mega/debris_pat.h"
#include "../mega/debris_scroll.h"

int go_polygon;
int flip_flop;
u8 *p;
int mode;
int looper;

int fuzzy, fuzzy_dest;

const vert_t cube_verts[] = {
 { -10 , -10, -10}, // 0
 {  10 , -10, -10}, // 1
 {  10 ,  15, -10}, // 2
 { -10 ,  15, -10}, // 3
 { -10 , -10,  10}, // 4
 {  10 , -10,  10}, // 5
 {  10 ,  15,  10}, // 6
 { -10 ,  15,  10}, // 7

 /*       
  *     0  1
  *    4  5
  *     3  2
  *    7  6
  *
  */
};

const tri_t cube_tris[] = {
 {5,6,4},
 {4,6,7},
 {5,1,6},
 {1,2,6},
 {1,0,3},
 {1,3,2},
 {0,4,7},
 {0,7,3},

 {2,3,6},
 {6,3,7},

 {1,5,0},
 {0,5,4}
};


void polygon_bottom(void) {

 if(go_polygon==1) {
  if(flip_flop == 0)
   do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)buffer, 0x100, 0x2000);
  else
   do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)buffer2, 0x100, 0x2000);
  go_polygon = 0;
 }
 pitch += float2fix(1.0f);
 yaw -= float2fix(0.3f);
}


void init_polygon(void) {
 u16 *p;
 int i;
 int x,y;
 current_pal =debris_color;
 current_pal_count = 32;

 for(i=0;i<3072;i++) {
  VRAM_WRITE(SPRITE_BASE+i, 0);
 }

 vram_write(0,debris_pat, 0x8000);
 vram_write(0xc000, debris_scroll, 0x1000);

 for(i=0;i<0x1000;i++)
  buffer[i] = 0;


 p = (u16 *)buffer;

 for(y=0, i=0;y<16;y++)
  for(x=0;x<16;x++, i++) 
   p[((x+12) + ((y+5)*64))] = 0x2000 | (i+8) | 0x8000;

 vram_write(0xe000, buffer, 0x1000);

 for(i=0;i<0x2000;i++) {
  buffer[i] = 0;
  buffer2[i] = 0;
 }

 current_object = cube_verts;
 pitch = 0;
 yaw = 0;
// x_scale = float2fix(1.0f);
// y_scale = float2fix(2.5f);

 x_scale = float2fix(1.7f);
 y_scale = float2fix(2.5f);
 x_shift = 32;
 y_shift = 64;
 z_shift = float2fix(40.0f);
 vert_count = 8;

 flip_flop = 0;
 go_polygon = 0;
 looper = 0;
 v_hook = polygon_bottom;
 mode = 0;
 fuzzy = 0;
 fuzzy_dest = 0;
 updating_palette = 0;
 VSRAM_WRITE(0,-20);
 VDP_SET_REGISTER(H_COUNTER,200);
 fade_to = 0;
}


void tick_polygon(void) {
 int i;
 int x,y, z_normal;
 u8 color;


 if(flip_flop == 1)
  p = buffer;
 else
  p = buffer2;

  __asm__(
   "move.l 	(p), %a1	\n"
   "move.w #0x7ff, %d0  	\n"
   "move.l #0x00000000, %d1	\n"
   "clear_screen_loop:		\n"
   "move.l %d1, (%a1)+		\n"
   "dbf %d0, clear_screen_loop	\n");

  rot_verts(pitch, yaw);


 for(i=0;i<12;i++) { 
#define V1 vert_rot[cube_tris[i].v1]
#define V2 vert_rot[cube_tris[i].v2]
#define V3 vert_rot[cube_tris[i].v3]

  z_normal = ((V2.x-V1.x) * (V3.y-V2.y)) -((V3.x-V2.x) * (V2.y-V1.y));
  color = 7-(abs(z_normal>>9)%7);
  color++;
  if(z_normal < 0) {
   flat_triangle(p, &cube_tris[i], color | (color<<4) );
  }

 }
 line(p, 15,90,64,89,0x99);
 line(p, 15,91,64,89,0x99);
 line(p, 15,92,64,89,0x99);

 line(p, 16,91,16,128,0x99);
 line(p, 15,91,16,128,0x99);




 go_polygon = 1;
 flip_flop^=1;


 looper++;

}
