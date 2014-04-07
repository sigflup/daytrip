#include <libmega.h>

#include "main.h"
#include "sprites.h"
#include "vert.h"
#include "stdlib.h"

#include "../mega/vector_floor_pat.h"
#include "../mega/vector_floor_scroll.h"
#include "../mega/vector_floor_pal.h"


#define NUM_VECTORS	16	
const vert_t verts_poly[] = {

 { -30,  30, -30},
 {  30,  30, -30},
 {  30, -30, -30},
 { -30, -30, -30}, 

 { -30,  30, 30},
 {  30,  30, 30},
 {  30, -30, 30},
 { -30, -30, 30}, 

 { -10,  30, 30},
 { 10,  30, 30},

 {  10, -30, 30},
 { -10, -30, 30},

 {  10, -30, -30},
 { -10, -30, -30},

 {  10, 30, -30},
 { -10, 30, -30},

//
 {-30, 10, -30},
 {-30,-10, -30}, 

 {30, 10, -30},
 {30,-10, -30}, 

 {30, 10, 30},
 {30,-10, 30}, 

 {-30, 10, 30},
 {-30,-10, 30} 


};

int sprite_ram_mutex;

int looper;

color_t vector_color[32];

const color_t color_map[8] = {
 { 7,7,0 },
 { 7,7,0 },
 { 7,7,0 },
 { 7,7,0 },
 { 0,0,7 },
 { 0,0,7 },
 { 0,0,7 },
 { 0,0,7 }
};

const int offset_map[8] = {
 3, 5, 2, 6, 4 , 7, 8, 1
};

void update_vector_balls_palette(void) {
 int i;
 for(i=0;i<8;i++) {
  vector_color[offset_map[i]].r = color_map[(i+(looper>>1))&7].r;
  vector_color[offset_map[i]].g = color_map[(i+(looper>>1))&7].g;
  vector_color[offset_map[i]].b = color_map[(i+(looper>>1))&7].b;
 }
}

void v_hook_vector_balls(void);

void init_vector_balls(void) {
 int i;
 v_hook = no_routine;
 h_hook = no_routine;
 VSRAM_WRITE(0, 0);
 VSRAM_WRITE(2, 0);

 current_pal = vector_color;
 current_pal_count = 32;

 for(i=0;i<0x3ff;i++)
  VRAM_WRITE(0xfc00+i, 0);

 for(i=1;i<32;i++) {
  vector_color[i].r = vector_floor_color[i].r;
  vector_color[i].g = vector_floor_color[i].g;
  vector_color[i].b = vector_floor_color[i].b;
 }

 vram_write(0, vector_floor_pat, 0x2400);
 vram_write(0xc000, vector_floor_scroll, 0x1000);

 for(i=0;i<0x1000;i++)
  VRAM_WRITE(i+0xe000, 0);
 for(i=0;i<80*2;i++)
  VRAM_WRITE(SPRITE_BASE+i, 0);
 looper = 0;
 update_vector_balls_palette();

 reset_sprites();

 pitch = 0;
 yaw = 0;
 x_scale = float2fix(3.0f);
 y_scale = float2fix(3.0f);
 x_shift = (140);
 y_shift = (112-16);
 z_shift = float2fix(70.0f);

 current_object = verts_poly;
 vert_count = NUM_VECTORS;
 sprite_ram_mutex = 0;
 v_hook = v_hook_vector_balls;
 updating_palette = 1;
 VDP_SET_REGISTER(H_COUNTER, 200);
 fade_to = 0;
}

void v_hook_vector_balls(void) {
 if(sprite_ram_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 80*8);
  sprite_ram_mutex = 0; 
 }
 pitch+=float2fix(1.5f);
 yaw+= float2fix(1.0f);
 looper--;

}

int cmp(vert_t *a, vert_t *b) {
 if(a->z < b->z) return -1;
 else
  return 1;
}

void tick_vector_balls(void) {
 int i;
 int j;
 int smallest;
 int smallest_id;
 update_vector_balls_palette();


 if(sprite_ram_mutex == 0) {
  rot_verts(pitch, yaw);
  qsort(vert_rot, NUM_VECTORS, sizeof(vert_t), cmp);
  clear_sprites();


  for(i=0;i<NUM_VECTORS;i++) {
   put_sprite(vert_rot[i].x, vert_rot[i].y, 3, 3, 0, 0, 1, 1, 0x110);
   }
  sprite_ram_mutex = 1;
 }

// pitch+=float2fix(1.0f);
// yaw+= float2fix(0.5f);
}
