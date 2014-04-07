#include <libmega.h>

#include "main.h"
#include "scene.h"
#include "sprites.h"


#include "../mega/forcer_2_pal.h"
#include "../mega/forcer_2_pat.h"
#include "../mega/forcer_2_scroll.h"
#include "../mega/forcer_2_texture.h"

const char *forcer_2_msg = 
 "while{playing{video{games{one{day{david{suddenly{started{to{feel{ill{{{{\0";

int forcer_2_msg_len;

int looper;
int vs_y, vs_dest;
int vs_x;

int sprite_ram_mutex;

void forcer_2_bottom(void) {
 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);
 if(sprite_ram_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 80*8);
  sprite_ram_mutex = 0;
 }

 VSRAM_WRITE(0, vs_y);
 VSRAM_WRITE(2, vs_y/2);


}

void init_forcer_2(void) {
 int i; 
 int x, y, sx, sy;
 u16 *w;

 v_hook = no_routine;
 h_hook = no_routine;

 VDP_SET_REGISTER(NAME_TABLE_SCROLLA_BASE, SCROLL_A_ADDR(0xc000));
 VDP_SET_REGISTER(NAME_TABLE_WINDOW_BASE,  SCROLL_A_ADDR(0x10000));
 VDP_SET_REGISTER(NAME_TABLE_SCROLLB_BASE, SCROLL_B_ADDR(0xe000));
 VDP_SET_REGISTER(SPRITE_ATTRIBUTE_BASE,   SPRITE_ADDR(SPRITE_BASE));
 VDP_SET_REGISTER(SCROLL_SIZE, SCROLL_WIDTH_64);


 VSRAM_WRITE(0,0);
 VSRAM_WRITE(2,0);

 current_pal = forcer_2_color;
 current_pal_count = 48;

 vram_write(0,forcer_2_pat, 0x3b00);
 for(i=0;i<0x4000;i++) {
  VRAM_WRITE(0x3b00+i,0);
 }
 vram_write(0x3b00, font, 3328); // 0x1d8
 vram_write(0xc000, forcer_2_scroll, 0x1000);

 w = (u16 *)forcer_2_texture;
 for(y=0;y<8;y++)
  for(x=0;x<8;x++) {
   for(sy = 0;sy<4;sy++)
    for(sx = 0;sx<8;sx++) {
     VRAM_WRITE(0xe000 + (((64*(y+(sy*8)))+(x+(sx*8)))*2), w[(64*y)+x]);
    }
  }
 

 for(i=0;i<244*2;i++)
  hscroll_buf[i] = 0;
 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);


 reset_sprites();
 for(i=0;i<80*2;i++) {
  VRAM_WRITE(SPRITE_BASE +i, 0);
 } 

 VDP_SET_REGISTER(H_COUNTER, 255);
 
 v_hook = forcer_2_bottom;
 vs_y = 0;
 vs_x = 500;
 vs_dest = 0;
 looper = 0;
 sprite_ram_mutex = 0;
 forcer_2_msg_len = strlen(forcer_2_msg);

 fade_to = 0;

}


void tick_forcer_2(void) {
 int i;
 int x;
 int y;
 if(looper >= 50) {
  vs_dest = 30;
 }
 if(sprite_ram_mutex == 0) {
  clear_sprites();
  for(i=0;i<forcer_2_msg_len;i++) {
   x = (i*16) + vs_x;
   y = 190 + (((cos_tab[x&0xff] *float2fix(10.0f))>>8)>>8);
   if((x>-16) && (x<(320+32))) {
    if(i<forcer_2_msg_len)
     put_sprite(x,y,1,1,0,0,2,1, 0x1d8 + ((forcer_2_msg[i]-'a')*4)); 
   }
  } 
  sprite_ram_mutex = 1;
 }

 for(i=0;i<244;i++) {
  hscroll_buf[(i*2)+1] =
   ((cos_tab[((i>>1)+looper)&0xff]*float2fix(80.0f))>>8)>>8;
 }
 vs_y+= ((vs_dest - vs_y)/8);

 vs_x-=2;
 looper++;
}
