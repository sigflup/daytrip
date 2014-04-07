#include <libmega.h>

#include "main.h"
#include "sprites.h"

#include "../mega/forcer_1_pal.h"
#include "../mega/forcer_1_pat.h"
#include "../mega/forcer_1_scroll.h"
#include "../mega/forcer_1_texture.h"
#include "../mega/font.h"

int looper;
int looper2;

int sprite_ram_mutex;

void forcer_1_bottom(void) {
 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);
 if(sprite_ram_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 80*8);
  sprite_ram_mutex = 0;
 }
 VSRAM_WRITE(2, looper2);
 looper2++; 
}

void init_forcer_1(void) {
 int i;
 int x, y, sx, sy;
 u16 *w;
 v_hook = no_routine;
 h_hook = no_routine;
 VSRAM_WRITE(0, 0);
 VSRAM_WRITE(2, 0);

 current_pal = forcer_1_color;
 current_pal_count = 48;

 VDP_SET_REGISTER(H_COUNTER, 1);
 vram_write(0, forcer_1_pat, 0x3700);
 vram_write(0x3700, font, 3328); //0x1b8
 vram_write(0xc000, forcer_1_scroll, 0x1000);

 w = (u16 *)forcer_1_texture;
 for(y=0;y<8;y++)
  for(x=0;x<8;x++) {
   for(sy = 0;sy<4;sy++)
    for(sx = 0;sx<8;sx++) {
     VRAM_WRITE(0xe000 + (((64*(y+(sy*8)))+(x+(sx*8)))*2), w[(64*y)+x]);

    }
 
  }

 for(i=0;i<80*8;i++) {
  VRAM_WRITE(SPRITE_BASE+i, 0);
  sprite_ram[i] = 0; 
 }
 looper = 0;
 looper2 = 0;
 sprite_ram_mutex = 0;
 v_hook = forcer_1_bottom;
 fade_to = 0;
}

typedef struct {
 int x, y, dest_x, dest_y;
 int pat;
} moving_sprite_t;

moving_sprite_t moving_sprite[100];
int moving_sprite_count;

void new_moving_sprite(int start_x,int start_y,int dest_x,int dest_y,int pat) {
 moving_sprite[moving_sprite_count].x = start_x;
 moving_sprite[moving_sprite_count].y = start_y;
 moving_sprite[moving_sprite_count].dest_x = dest_x;
 moving_sprite[moving_sprite_count].dest_y = dest_y;
 moving_sprite[moving_sprite_count].pat = ((pat-'A') *4) + 0x1b8;
 moving_sprite_count++;
}

typedef struct {
 int looper_pos;
 int pat;
 int x, y;
} story_t;

const story_t story[] = {
 { 10, 'O', 200, 50},
 { 20, 'U', 216, 50},
 { 30, 'R', 232, 50},

 { 40, 'S', 210, 70},
 { 50, 'T', 210+(16*1), 70},
 { 60, 'O', 210+(16*2), 70},
 { 70, 'R', 210+(16*3), 70},
 { 80, 'Y', 210+(16*4), 70},

 { 90,  'B', 214, 90},
 { 100, 'E', 214+(16*1), 90},
 { 110, 'G', 214+(16*2), 90},
 { 120, 'I', 214+(16*3), 90},
 { 130, 'N', 214+(16*4), 90},
 { 140, 'S', 214+(16*5), 90},

 { 150, 'W', 200, 110},
 { 160, 'I', 200+(16*1), 110},
 { 170, 'T', 200+(16*2), 110},
 { 180, 'H', 200+(16*3), 110},

 { 190, 'O', 200+(16*5), 110},
 { 200, 'U', 200+(16*6), 110},
 { 210, 'R', 200+(16*7), 110},

 { 220, 'F', 214, 130},
 { 230, 'R', 214+(16*1), 130},
 { 240, 'I', 214+(16*2), 130},
 { 250, 'E', 214+(16*3), 130},
 { 260, 'N', 214+(16*4), 130},
 { 270, 'D', 214+(16*5), 130},

 { 280, 'D', 210, 150},
 { 290, 'A', 210+(16*1), 150},
 { 300, 'V', 210+(16*2), 150},
 { 310, 'I', 210+(16*3), 150},
 { 320, 'D', 210+(16*4), 150},




 {0xfffff, 'p',100,100}
};

int story_pos = 0;
int begin_moving_sprite = 0;

void tick_forcer_1(void) {
 int i;
 if(looper == story[story_pos].looper_pos) {
  new_moving_sprite(-10, -10, story[story_pos].x,story[story_pos].y,
    story[story_pos].pat);
  story_pos++;
 }



 for(i=0;i<244;i++) 
  hscroll_buf[(i*2)+1] = looper;
 looper++;

 if(sprite_ram_mutex == 0) {
  clear_sprites();
  for(i=0;i<moving_sprite_count;i++) 
   put_sprite(moving_sprite[i].x, moving_sprite[i].y, 1, 1, 0,0, 2,1,
     moving_sprite[i].pat);

  for(i=begin_moving_sprite;i<moving_sprite_count;i++) {
   moving_sprite[i].x += ((moving_sprite[i].dest_x - moving_sprite[i].x) / 8);
   moving_sprite[i].y += ((moving_sprite[i].dest_y - moving_sprite[i].y) / 8);
  }
  sprite_ram_mutex = 1;
 }
}
