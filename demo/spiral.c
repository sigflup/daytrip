#include <libmega.h>

#include "main.h"
#include "sprites.h"

#include "../mega/spiral_pal.h"
#include "../mega/spiral2_pat.h"
#include "../mega/spiral2_scrolla.h"
#include "../mega/spiral2_scrollb.h"

s16 looper;
s16 h_pos;


typedef struct {
 int pos;
 char message[20];
} shouts_t;

#define START	80
#define DELAY	40
const shouts_t shouts[] = {
 { DELAY*1, "what" },
 { DELAY*2, "you" },
 { DELAY*3, "need" },
 { DELAY*4, "is" },
 { DELAY*5, "a" },
 { DELAY*6, "steady" },
 { DELAY*7, "diet" },
 { DELAY*8, "of" },
 { DELAY*9, "twisters" },
 { DELAY*10,"" },
 { DELAY*11,"" },
 { DELAY*12,"" },
 { DELAY*13,"" },
 { DELAY*14,"" },
 { DELAY*15,"" },
 { DELAY*16,"" },
 { DELAY*17,"" },
 { DELAY*18,"" },
 { DELAY*19,"" },
 { DELAY*20,"" },
 { DELAY*21,"" },

};

int shouts_pos;

extern const u8 font[];

int sprite_ram_mutex;

void h_hook_spiral(void) {
#define H_OFF \
 cos_tab[(h_pos + looper)&0xff]
// h_pos++; 
 SET_VSRAM_ADDRESS(0);
 address = (u16 *)VDP_DATA;
 *address = -cos_tab[(h_pos + looper)&0xff];
 *address = 0; 
 h_pos++; 

}

void v_hook_spiral(void) {
 h_pos = 0;
 pal_mutex = 1;
 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x380);
 if(sprite_ram_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 80);
 sprite_ram_mutex = 0;
 }  
 pal_mutex = 0;
}

void init_spiral(void) {
 int i;
 v_hook = no_routine;
 h_hook = no_routine;
 current_pal = spiral2_color;
 current_pal_count = 34;

 VDP_SET_INC(2);
 VDP_SET_REGISTER(H_COUNTER, 2);

 looper = 0;
 vram_write(0, spiral2_pat, 0x6c00);
 vram_write(0x7600, font, 3328);
 vram_write(0xc000, spiral2_scrolla, 0x1000);
 vram_write(0xe000, spiral2_scrollb, 0x1000);
 for(i=0;i<0x3ff;i++) {
  VRAM_WRITE(0xfc00+i, 0);
 }
 reset_sprites();
 for(i=0;i<80*2;i++) {
  VRAM_WRITE(SPRITE_BASE+i, 0);
 }
 sprite_ram_mutex = 0;
 shouts_pos = 0;
 h_hook = h_hook_spiral;
 v_hook = v_hook_spiral;

 updating_palette = 0;
 dma_corrupt = 1;
 fade_to  = 0;
}

void word(int y, char *in) {
 int j, i;
 j = strlen(in);
 for(i=0;i<j;i++)
  put_sprite((70-((j/2)*8))+(i*16),y,1,1,0,0,2,1, 0x3b0 + ((in[i] - 'a')*4));
}


void tick_spiral(void) {
 int i;
 looper++; 

 for(i=0;i<244;i++) {
  hscroll_buf[(i*2)]=
   (((cos_tab[((i>>2)+looper)&0xff]*float2fix(40.0f))>>8)>>8)+70;
  hscroll_buf[(i*2)+1] = 0;
 } 

 if(sprite_ram_mutex == 0) {
  clear_sprites();

  if((shouts[shouts_pos].pos+START) <= looper) {
   word(150, shouts[shouts_pos].message); 
   shouts_pos++;
  }

  sprite_ram_mutex = 1;
 } 

}
