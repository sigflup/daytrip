#include <libmega.h>

#include "main.h"

#include "../mega/secret_pal.h"
#include "../mega/secret_pat.h"
#include "../mega/secret_scroll.h"


void init_secret(void) {
 int i;
 vram_write(0, secret_pat, 0x7100);
 vram_write(0xc000, secret_scroll, 0x1000);
 current_pal = secret_color;
 current_pal_count = 16;

 VDP_SET_REGISTER(MODE_SET_3, 0);
 VDP_SET_REGISTER(NAME_TABLE_SCROLLA_BASE, SCROLL_A_ADDR(0xc000));
 VDP_SET_REGISTER(NAME_TABLE_WINDOW_BASE,  SCROLL_A_ADDR(0x10000));
 VDP_SET_REGISTER(NAME_TABLE_SCROLLB_BASE, SCROLL_B_ADDR(0xe000));
 VDP_SET_REGISTER(SPRITE_ATTRIBUTE_BASE,   SPRITE_ADDR(SPRITE_BASE));
 VDP_SET_REGISTER(SCROLL_SIZE, SCROLL_WIDTH_64);


 for(i=0;i<0x3ff;i++)
  VRAM_WRITE(0xfc00+i, 0);
 for(i=0;i<80*8;i++) 
  VRAM_WRITE(SPRITE_BASE+i, 0);

 for(i=0;i<0x1000;i++)
  VRAM_WRITE(i+0xe000, 0);



 VSRAM_WRITE(0, 0); 
 VSRAM_WRITE(2, 0);

 dma_corrupt = 0;

 fade_to = 0;
}

void tick_secret(void) {

}
