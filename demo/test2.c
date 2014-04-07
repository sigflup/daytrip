#include <libmega.h>

#include "main.h"
#include "scene.h"
#include "sprites.h"

#include "test2_tab.h"

#define MAP_WIDTH	256


#include "../mega/test3_pal.h"
#include "../mega/test3_pat.h"
#include "../mega/test3_scroll.h"
#include "../mega/test3_scrollb.h"



int looper;
int *dist_map;
int *dist_map2;

int *front, *back;
int flip_flop;


int scanline;

int sprite_ram_mutex;

u8 theta;

typedef struct {
 int clear;
 int target, off_x, delay;
 u8 *msg;
} word_t;

word_t words[6];
int word_count;

void add_word(u8 *msg) {
 words[word_count].clear = 1;
 words[word_count].target = 100;
 words[word_count].off_x = -200;
 words[word_count].msg = msg;
 words[word_count].delay = 0;
 word_count++;
}

void tick_words(void) {
 int i,j;
 for(i=0;i<word_count;i++) {
  if(words[i].clear == 1) {
   words[i].off_x+=(words[i].target-words[i].off_x)/5;
   if(abs(words[i].off_x-words[i].target)<10)
    words[i].delay++;
   if(words[i].delay >= 30)
    words[i].target = 300;
   if(words[i].off_x == 300)
    words[i].clear = 0;
   for(j = 0;j<strlen(words[i].msg);j++) {
#define Y \
   ((words[i].off_x+(j*16)) - (strlen(words[i].msg)*8))
    put_sprite(270,Y,1,1,0,0,2,1,0x210 + (( words[i].msg[j] - 'a') * 4));
   }
  }
 } 
}

const u8 msg1[] = { "how" };
const u8 msg2[] = { "about" };
const u8 msg3[] = { "a" };
const u8 msg4[] = { "barrel" };
const u8 msg5[] = { "rotator"};


void h_bottom_test2(void) {
  SET_VSRAM_ADDRESS(0);
  address = (u16 *)VDP_DATA;
  *address =-scanline - (scanline-front[scanline]);
  *address = looper;
  /* VSRAM_WRITE(0, -scanline - (scanline-dist_map[scanline]));
 VSRAM_WRITE(4, -scanline - (scanline-dist_map2[scanline])); */

 scanline++;
}

void v_bottom_test2(void) {
 scanline = 0;
 if(sprite_ram_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE,  80*8);
  sprite_ram_mutex = 0; 
 } 
 theta++;
 if(theta == 0)
  theta = 1;
 if(theta==0xff)
  theta = 1;


 looper++;
}

int looper2;


void init_test2(void) {
 int i;
 u16 *w;
 u8 *b;
 int x,y,sx,sy;
 current_pal = test3_color;
 current_pal_count = 48;
 VDP_SET_REGISTER(H_COUNTER, 1);

 vram_write(0, test3_pat, 0x4000);
 vram_write(0xc000, test3_scroll, 0x1000);

 w = (u16 *)test3_scrollb;
 for(y=0;y<8;y++)
  for(x=0;x<8;x++) {
   for(sy = 0;sy<4;sy++)
    for(sx = 0;sx<8;sx++) {
     VRAM_WRITE(0xe000 + (((64*(y+(sy*8)))+(x+(sx*8)))*2), w[(64*y)+x]);

    }
 
  }


 scanline = 0;

 for(i=0;i<0x3ff;i++)
  VRAM_WRITE(0xfc00+i, 0);
 for(i=0;i<80*8;i++) 
  VRAM_WRITE(SPRITE_BASE+i, 0);

 vram_write(0x4200,font,3328); // 0x210

 sprite_ram_mutex = 0;
 dist_map = buffer;
 dist_map2 = buffer2;
 front = dist_map2;
 back = dist_map;
 flip_flop = 0;
 theta = 0;

 updating_palette = 1;

 reset_sprites();
/* for(i = 0;i<strlen(msg1);i++) {
  put_sprite(20,20+(i*16),1,1,0,0,2,1,0x210 + (( msg1[i] - 'a') * 4));
 }
 for(i = 0;i<strlen(msg2);i++) {
  put_sprite(270,20+(i*16),1,1,0,0,2,1,0x210 + (( msg2[i] - 'a') * 4));
 }
 for(i = 0;i<strlen(msg3);i++) {
  put_sprite(290,20+(i*16),1,1,0,0,2,1,0x210 + (( msg3[i] - 'a') * 4));
 }
 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 
   (sprite_index)*8);
*/
 sprite_ram_mutex = 0;
 word_count = 0;

 v_hook = v_bottom_test2;
 h_hook = h_bottom_test2;

 looper2 = 0;
// add_word(msg1);
 fade_to = 0;

 
}


void tick_test2(void) {
 int i;

#define START	40
#define DELAY	50
 if(looper2 == START)
  add_word(msg1);
 if(looper2 == START+DELAY)
  add_word(msg2);
 if(looper2 == START+(DELAY*2))
  add_word(msg3);
 if(looper2 == START+(DELAY*3))
  add_word(msg4);
 if(looper2 == START+(DELAY*4))
  add_word(msg5);


 looper2++;


 for(i=0;i<128;i++) {
  back[i] = test2_tab[(i+10) + (128 * theta) ]; 
 }

 if(sprite_ram_mutex == 0) {
  clear_sprites();

  tick_words();
  sprite_ram_mutex = 1;
 }


 flip_flop^=1;
 if(flip_flop == 1) {
  front = dist_map;
  back = dist_map2;
 } else {
  front = dist_map2;
  back = dist_map;
 }
/*
 theta++;
 if(theta == 0)
  theta = 1;
 if(theta==0xff)
  theta = 1;
*/
}
