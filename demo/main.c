#include <libmega.h>
#include "main.h"
#include "scene.h"
#include "sprites.h"

#include "../z80/prog.h"

u16 hscroll_buf[244*2];

const u8 secret_msg[] = {
 UP,
 DOWN,
 LEFT,
 RIGHT,
};

int secret_pos;

u8 controller_buffer[7];
u8 controller, lcontroller;

int pal_mutex;
color_t *current_pal;
int current_pal_count;

int brightness, fade_to;

int updating_palette;
int dma_corrupt;


u32 vgm_pos;

int song_pos, song_ticks;

u8 buffer[0x2000];
u8 buffer2[0x2000];

void (*v)(void);


s16 rand_short(s32 *seed) {
 return ((*seed = *seed * 214013L + 2531011L)>>16)&0x7fff;
}

void update_vgm_pos(void) {
 u8 *p;
 p=(u8 *)&vgm_pos;
 GET_Z80BUS;
 p[3] = *(u8 *)(Z80_RAM+0x50);
 p[2] = *(u8 *)(Z80_RAM+0x51);
 DROP_Z80BUS;
}


int last_brightness = 0;

void update_palette(void) {
 int i;
 int nr,ng,nb;
 if((last_brightness != brightness) ||
    (updating_palette==1)) {
//  SET_CRAM_ADDRESS(0);
//  address2 = (u16 *)VDP_DATA;
  for(i=0;i<current_pal_count;i++) {

   nr = current_pal[i].r + brightness;
   CLAMP(nr, 0, 7);
   ng = current_pal[i].g + brightness;
   CLAMP(ng, 0, 7);
   nb = current_pal[i].b + brightness;
   CLAMP(nb, 0, 7);

   for(;;)
    if(pal_mutex == 0) break;

   VDP_SET_PAL2(i, nr, ng, nb);
   if(dma_corrupt == 1) {
    VDP_SET_PAL2(i, nr, ng, nb); 
   }

//   *address2 = VDP_CRAM_COLOR(nr,ng,nb);
  }
 }
 last_brightness = brightness;
 if(brightness > fade_to) brightness--;
 if(brightness < fade_to) brightness++;
// v();
} 

void (*proc[])(void) = {

// init_prescrip,
// tick_prescrip, 

 init_sega,
 tick_sega,
 init_forcer_1,
 tick_forcer_1,
 init_vax,
 tick_vax,
 init_forcer_2,
 tick_forcer_2,
 init_forcer_phone,
 tick_forcer_phone,
 init_plazma,
 tick_plazma,
 init_waitroom,
 tick_waitroom,
 init_vertical,
 tick_vertical,
 init_indoctor,
 tick_indoctor,
 init_test, 
 tick_test,
 init_spiral,
 tick_spiral, 
 init_prescrip,
 tick_prescrip,
 init_vector_balls,
 tick_vector_balls,
 init_nurse,
 tick_nurse,
 init_polygon,
 tick_polygon,
 init_test2,
 tick_test2,
 init_pillz,
 tick_pillz
};

const int scenes[] = {
// 0xffff,

 0x230, //sega end
 0x600, //forcer_1 end
 0xb30, //vax end
 0x1300,//forcer_2 end
 0x1800,//forcer_phone_end
 0x2100,//plazma end
 0x2400,//waitroom end
 0x2800,//vertical end
 0x2900,//indoctor end
 0x2a00,//test end
 0x2e00,//spiral end
 0x2fff,//prescrip end
 0x3200,//vector_balls end
 0x3400,//nurse end
 0x3a00,//polygon end
 0x4000,//test2 end
 0x4f5b,

 0xffff 
};

void secret(void) {
 int i;
 GET_Z80BUS;
 h_hook = no_routine;
 v_hook = no_routine;
 fade_to = -8;
 brightness = -8;
 update_palette();
 init_secret();
 for(i=0;i<0xff;i++) {
  FM1_WRITE(i,0);
  FM2_WRITE(i,0);
 }
 for(;;) {
  SYNC;
  update_palette();

  tick_secret();

 }
}

int main(void) {
 v = no_routine;
 v_hook = no_routine;
 h_hook = no_routine;
 pal_mutex = 0;

 VDP_SET_REGISTER(MODE_SET_1, 0x16);
 VDP_SET_REGISTER(NAME_TABLE_SCROLLA_BASE, SCROLL_A_ADDR(0xc000));
 VDP_SET_REGISTER(NAME_TABLE_WINDOW_BASE,  SCROLL_A_ADDR(0x10000));
 VDP_SET_REGISTER(NAME_TABLE_SCROLLB_BASE, SCROLL_B_ADDR(0xe000));
 VDP_SET_REGISTER(SPRITE_ATTRIBUTE_BASE,   SPRITE_ADDR(SPRITE_BASE));
 VDP_SET_REGISTER(0x06, 0x00);
 VDP_SET_REGISTER(BACKGROUND_COLOR, 0x00);
 VDP_SET_REGISTER(0x08, 0x01);
 VDP_SET_REGISTER(0x09, 0x01);
 VDP_SET_REGISTER(H_COUNTER, 1);
 VDP_SET_REGISTER(MODE_SET_3, HORIZONTAL_LINE);
 VDP_SET_REGISTER(MODE_SET_4, CELLW_40);
 VDP_SET_REGISTER(HSCROLL_BASE, HSCROLL_ADDR( 0xfc00));
 VDP_SET_REGISTER(0x0e, 0x00);
 VDP_SET_INC(0x02);
 VDP_SET_REGISTER(SCROLL_SIZE, SCROLL_WIDTH_64);
 VDP_SET_REGISTER(WINDOW_X, 0x00);
 VDP_SET_REGISTER(WINDOW_Y, 0x00);
 VDP_SET_REGISTER(19, 0x00);
 VDP_SET_REGISTER(20, 0x00);
 VDP_SET_REGISTER(21, 0x00);
 VDP_SET_REGISTER(22, 0x00);

 z80_write(0,z80_prog, Z80_PROG_LEN);
 z80_reset();

 brightness = -8;
 fade_to = -8;
 updating_palette = 0;
 dma_corrupt = 0; 

 secret_pos = 0;
 lcontroller = 0;
 song_pos = 0;
 song_ticks = 0;

 VDP_SET_REGISTER(MODE_SET_2, 0x74);

 proc[0]();//  init_sega();
 for(;;) {
  SYNC;
  update_palette();
  
  if(song_ticks == 0)  {
   h_hook = no_routine;
   v_hook = no_routine;
   fade_to = -8;
   brightness = -8;
   update_palette();
   proc[song_pos*2]();
   song_ticks = 0;
  }

  update_vgm_pos();
  
  proc[(song_pos*2)+1](); 

  song_ticks++;
  if(vgm_pos>=scenes[song_pos]) {
   song_pos++;
   song_ticks = 0;
  }


  lcontroller = controller;
  controller = poll_ctrl1();
  if((controller != lcontroller) && (controller != 0)) {
   if(secret_msg[secret_pos] == controller) 
    secret_pos++;
   else
    secret_pos = 0;
   if(secret_pos == 4) secret();
  } 
 }

}

