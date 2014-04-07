#include <libmega.h>

#include "main.h"

#include "sprites.h"

u16 sprite_ram[80*4];

int sprite_index;

prev_sprite_t prev_sprite;

void clear_sprites(void) {
 sprite_index = 0;
}

void reset_sprites(void) {
 int i;
 sprite_index = 0;
 for(i=0;i<80*4;i++)
  sprite_ram[i] = 0;
}

void put_sprite(int x, int y, int w, int h, int v_flip, int h_flip, int pal,
  int prio, int index) {
 x+=128;
 y+=128;


 if(sprite_index !=0) {
  sprite_ram[((sprite_index-1)*4)  ] =prev_sprite.y&0x7ff;
  sprite_ram[((sprite_index-1)*4)+1] =((prev_sprite.h&3)<<8)|
   ((prev_sprite.w&3)<<10)|(sprite_index&0x7f); 
  sprite_ram[((sprite_index-1)*4)+2] =((prev_sprite.prio&1)<<15) | 
   ((prev_sprite.pal&3)<<13) | 
   ((prev_sprite.v_flip&1)<<12) | ((prev_sprite.h_flip&1)<<11) | 
   (prev_sprite.index&0x7ff);
  sprite_ram[((sprite_index-1)*4)+3] =prev_sprite.x&0x7fff;
 } 

 prev_sprite.x = x;
 prev_sprite.y = y;
 prev_sprite.w = w;
 prev_sprite.h = h;
 prev_sprite.v_flip = v_flip;
 prev_sprite.h_flip = h_flip;
 prev_sprite.pal = pal;
 prev_sprite.prio = prio;
 prev_sprite.index = index;


 sprite_ram[(sprite_index*4)  ] =y&0x7ff;
 sprite_ram[(sprite_index*4)+1] =((h&3)<<8)|
  ((w&3)<<10)|(x&0x7f); 
 sprite_ram[(sprite_index*4)+2] =((prio&1)<<15)|((pal&3)<<13) | 
  ((v_flip&1)<<12) | ((h_flip&1)<<11) | (index&0x7ff); 
 sprite_ram[(sprite_index*4)+3] =x&0x7fff;

 sprite_index++; 
}
