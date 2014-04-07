#include <libmega.h>

#include "main.h"

#include "../mega/waitroom_pal.h"
#include "../mega/waitroom_pat.h"
#include "../mega/waitroom_scroll.h"


void init_waitroom(void) {
 vram_write(0, waitroom_pat, 0x2800);
 vram_write(0xc000, waitroom_scroll, 0x1000);
 current_pal = waitroom_color;
 current_pal_count = 16;

 fade_to = 0;
}

void tick_waitroom(void) {

}
