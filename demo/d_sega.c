#include <libmega.h>

#include "main.h"
#include "sprites.h"

#include "../mega/sega_pal.h"
#include "../mega/sega_pat.h"
#include "../mega/sega_scroll.h"
#include "../mega/sega_scrollb.h"


extern const u8 font[];

const char message[][20] = {
 "code",
 "sigflup",
 "{",
 "gfx",
 "forcer",
 "{",
 "music",
 "groovemaster303",
 "jredd",
 "{",
 "in{the{year",
 "2013"
};

int sprite_ram_mutex;
int looper, pals;
int locate, coord_y, d_y;

void sega_bottom(void) {
 if(sprite_ram_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 80*8);
  sprite_ram_mutex = 0; 
 }
}

int init_sega(void) {
 int i,j;
 v_hook = no_routine;
 h_hook = no_routine;
 VSRAM_WRITE(0, 0);
 VSRAM_WRITE(2, 0);



 current_pal = sega_color;
 current_pal_count = 48;


 vram_write(0, sega_pat, 0x5900);

 VDP_SET_REGISTER(H_COUNTER,200);

 VDP_SET_REGISTER(AUTO_INCREMENT, 2);
 SET_VRAM_ADDRESS(0xc000);
 j = 0x00ff;
 address = (u16 *)VDP_DATA;
 for(i=0;i<0x1000;i+=2)
  *address = sega_scroll[i+1] | (sega_scroll[i]<<8) | 0x8000;

 vram_write(0x5900,font, 3328); // 0x2c8
 vram_write(0xe000, sega_scrollb, 0x1000);


 looper = 0;
 sprite_ram_mutex = 0;
 coord_y = 224;
 d_y = 0;
 locate = 100;
 pals = 0;
 v_hook = sega_bottom;
 fade_to = 0;

}

void sega_word(char *in,int y, int pal) {
 int len,i,x;
 len = strlen(in);
 for(i=0;i<len;i++) {
  x = (160 - ((len * 16)/2)) + (i*16);
  switch(in[i]) {
   case '0':
    put_sprite(x, y, 1,1,0,0,pal,0, (('o' - 'a')*4) + 0x2c8);
    break;
   case '1':
    put_sprite(x, y, 1,1,0,0,pal,0, (('i' - 'a')*4) + 0x2c8);
    break;
   case '2':
    put_sprite(x,y,  1,1,0,1,pal,0, (('s' - 'a')*4) + 0x2c8);
    break;
   case '3':
    put_sprite(x, y, 1,1,0,1,pal,0, (('e' - 'a')*4) + 0x2c8);
    break;
   default:
    put_sprite(x, y, 1,1,0,0,pal,0, ((in[i] - 'a')*4) + 0x2c8);
    break;
  }
 }
}

void tick_sega(void) {
 int i, len, pal; 
#define DELAY 170	
 if(looper == DELAY) {
  locate = 100 - 48;
  pals++;
 }
 if(looper == DELAY*2) {
  locate = 100 - (48*2);
  pals++;
 }

 if(sprite_ram_mutex == 0) {
  clear_sprites();

  for(i=0;i<9;i++) {
   if((pals == 0) && ((i == 0) || (i==1))) { pal = 2; goto done; } else pal = 3;
   if((pals == 1) && ((i == 3) || (i==4))) { pal = 2; goto done; } else pal = 3;
   if((pals == 2) && 
     ((i == 6) || (i==7) || (i==8))){ pal = 2; goto done; } else pal = 3;
done: 
 
   sega_word(message[i], coord_y + (i*16),pal);
  }
  sprite_ram_mutex = 1;
 }
 if(coord_y > locate) d_y--;
 if(coord_y < locate) d_y++;
 if((d_y>0) && ((looper&1)==1))
  d_y--;
 if((d_y<0) && ((looper&1)==1))
  d_y++;


 coord_y += d_y; 
 looper++;
}
