#include <libmega.h>

#include "main.h"

#include "../mega/nurse_pal.h"
#include "../mega/nurse_pat.h"
#include "../mega/nurse_scroll.h"


void init_nurse(void) {
 int i;
 vram_write(0, nurse_pat, 0x1600);
 vram_write(0xc000, nurse_scroll, 0x1000);
 current_pal = nurse_color;
 current_pal_count = 16;

 for(i=0;i<80*8;i++) 
  VRAM_WRITE(SPRITE_BASE+i, 0);


 fade_to = 0;
}

void tick_nurse(void) {

}
