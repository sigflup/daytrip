#include <libmega.h>

#include "main.h"
#include "sprites.h"

#include "../mega/door_pal.h"
#include "../mega/door_pat.h"
#include "../mega/door_scroll.h"

extern const u8 font[];

int sprite_ram_mutex;

int looper;
int looper_b;
int jiggle_x, jiggle_y;


void indoctor_v(void) {
 if(sprite_ram_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 80*8);
  sprite_ram_mutex = 0;
 }
}

void init_indoctor(void) {
 vram_write(0, door_pat, 0x1b00);
 vram_write(0x1b00, font, 3328); // 0xd8
 vram_write(0xc000, door_scroll, 0x1000);
 current_pal = door_color;
 current_pal_count = 32;
 dma_corrupt = 0;
 sprite_ram_mutex == 0;
 reset_sprites();
 updating_palette = 0;
 seed(23);
 looper_b = 0;
 jiggle_x = 5;
 jiggle_y = -4;
 v_hook = indoctor_v;
 looper = 0;
 fade_to = 0;
}

void put_knock(int x, int y) {
  put_sprite(x,y, 1,1,0,0,1,0, 0xd8 +        (('k' - 'a') * 4) );
  put_sprite(x+16,y, 1,1,0,0,1,0, 0xd8 +     (('n' - 'a') * 4) );
  put_sprite(x+(16*2),y, 1,1,0,0,1,0, 0xd8 + (('o' - 'a') * 4) );
  put_sprite(x+(16*3),y, 1,1,0,0,1,0, 0xd8 + (('c' - 'a') * 4) );
  put_sprite(x+(16*4),y, 1,1,0,0,1,0, 0xd8 + (('k' - 'a') * 4) );


}



void tick_indoctor(void) {

 
 if(sprite_ram_mutex == 0 && looper_b < 2) {
  reset_sprites();
  if(looper > 30)
   if((looper & 0x1f) < 0xf )
     put_knock(144+jiggle_x, 66+jiggle_y);

  if(looper > 90) {
   looper =0;
   jiggle_x = (get_random()&0xf)-7;
   jiggle_y = (get_random()&0xf)-7;
   looper_b++;
  }
  sprite_ram_mutex = 1;
 }

 looper++;
}
