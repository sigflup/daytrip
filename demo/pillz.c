#include <libmega.h>

#include "main.h"
#include "sprites.h"

#include "../mega/pillz_scrollb.h"
#include "../mega/pillz_pal.h"
#include "../mega/pillz_pat.h"
#include "../mega/pillz_scroll.h"

int looper;
int top, bot;
int scanline;

int pill_count;
int pillx, pilly;
int pill_fall;
int pill_speed;
int pill_rot;
int looper_stop;


char pillz_msg1[] = "thank";
char pillz_msg2[] = "you";
char pillz_msg3[] = "for";
char pillz_msg4[] = "watching";

char *current_msg;

extern const u8 font[];

int r_bar, g_bar, b_bar;

int r_looper, g_looper, b_looper;

int bright_tick;

int sprite_ram_mutex;

u16 bars_buf[224];

void pillz_h(void) {
 if(scanline == 0 ||
    scanline == 1 ||
    scanline == 2) {
  VSRAM_WRITE(0,top);
 } else
#define CONST 	68
  if(scanline>=(CONST-(top/2))) {
   if(scanline<=(CONST+(-bot))) {
    VSRAM_WRITE(0,-scanline - (scanline-136));
   } else {
    VSRAM_WRITE(0,bot);
   }
  }

 CRAM_WRITE(4, bars_buf[scanline]);
 scanline++;
}

void pillz_v(void) {
 if(sprite_ram_mutex ==1 ) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)sprite_ram, SPRITE_BASE, 80*8);
  sprite_ram_mutex = 0;
 }
 do_dma(DMA_68K, DMA_DST_VRAM,(u32 *)hscroll_buf, 0xfc00, 224*2*2);
 scanline = 0;
}

int move_down;

void init_pillz(void) {
 int x,y,sx,sy,i;
 h_hook = no_routine;
 v_hook = no_routine;
 current_pal = pillz_color;
 current_pal_count = 16;
 dma_corrupt = 0;
 updating_palette = 0;
 VSRAM_WRITE(0, 0);
 VSRAM_WRITE(2, 0);
 VDP_SET_REGISTER(SCROLL_SIZE, SCROLL_WIDTH_64 | SCROLL_HEIGHT_64);

 for(y=0;y<8;y++)
  for(x=0;x<8;x++) {
   for(sy = 0;sy<4;sy++)
    for(sx = 0;sx<8;sx++) {
     VRAM_WRITE(0xe000 + (((64*(y+(sy*8)))+(x+(sx*8)))*2), pillz_scrollb[(((64*y)+x)*2)+1]);

    }
 
  }

 for(x=0;x<0x1000/2;x++) {
  VRAM_WRITE(0xc000+(x*2), 
    (x < 1152 ? 0x8000 : 0x0000)|
    pillz_scroll[(x*2)+1] |(pillz_scroll[(x*2)]<<8));
 } 
 vram_write(0, pillz_pat, 0x3a00);
 vram_write(0x3a00, font, 3328); // 0x1d0

 bot = 0;
 move_down = 0;

 VDP_SET_REGISTER(MODE_SET_3, HORIZONTAL_CELL);


 looper = 0;
 bright_tick = 0;
 fade_to = 0;
 brightness = 0;
 last_brightness = 0;

 SET_CRAM_ADDRESS(0);
 address2 = (u16 *)VDP_DATA;
 for(i=0;i<48;i++) {
  *address2 = VDP_CRAM_COLOR(pillz_color[i].r,
                             pillz_color[i].g,
		             pillz_color[i].b);
 }
 sprite_ram_mutex = 0;
 pillx = 0;
 pilly = 0;
 pill_fall = 0;
 pill_speed = 0;
 pill_rot = 0;
 pill_count = 0;
 looper_stop = 0;
 current_msg = (char *)0;
 reset_sprites();
 h_hook = pillz_h;
 v_hook = pillz_v;


}

int looper2;

void tick_pillz(void) {
 int sc;
 int i,j,k;
 int r,g,b;
 int l;
#define CAP 100
#define LEN 40 
#define MOVE_LEN 32 

#define MOVE_LOCK (MOVE_LEN/2) 
 if(looper> (CAP-LEN-MOVE_LEN)) {
  if(looper < (CAP - MOVE_LEN))
   sc = looper&0x7;
  else {
   move_down=
   ((sin_tab[(looper-(CAP-MOVE_LEN))*4]*(float2fix((float)(0xbe/2))>>8))>>8)*2; 
  }
 }else
  sc = 0;

 if(looper == CAP) {
  looper = 0;
  move_down = 0;
  pill_count++;
 }

/* if((brightness == 0) &&( bright_tick == 0)) {
  h_hook = pillz_h;
  v_hook = pillz_v;
  bright_tick = 1;
 } */
 if(looper_stop == 0)
  top = sc; //+30;
 else 
  top+=6;

 r_looper+=5;
 g_looper+=3;
 b_looper+=4;
 r_bar = (((cos_tab[r_looper&0xff]*float2fix(30.0f))>>8)>>8)+50;
 g_bar = (((cos_tab[g_looper&0xff]*float2fix(30.0f))>>8)>>8)+50;
 b_bar = (((cos_tab[b_looper&0xff]*float2fix(30.0f))>>8)>>8)+50;

 for(i=0;i<224;i+=8) {
  hscroll_buf[(i*2)+1] = looper2<<1;
  if(i> 137)
   hscroll_buf[i*2] = move_down;
  else
   hscroll_buf[i*2] = 0;
 }

 bot = -(move_down /6);

 for(i=0;i<224;i++) {

  k = abs((r_bar-i)>>4);
  bars_buf[i] = VDP_CRAM_COLOR(7-k,0,0);
  k = abs((g_bar-i)>>4);
  bars_buf[i]|= VDP_CRAM_COLOR(0,7-k,0);
  k = abs((b_bar-i)>>4);
  bars_buf[i] |= VDP_CRAM_COLOR(0,0,7-k);

 }

 if(looper == (CAP-(LEN/2)-MOVE_LEN))
  pill_fall = 1;
 if(looper > CAP-(MOVE_LEN/2)) {
  pillx = 0;
  pilly = 0;
  pill_fall = 0;
  pill_speed = 0;
  pill_rot = 0;
 }

 if(pill_fall == 1) {
  pilly+=pill_speed;
  pill_speed+=2;
 }

#define FLOOR	(198-74)
 if(pilly >= FLOOR) {
  pill_fall = 0;
  pill_speed = 0;
  pilly = FLOOR;
  pill_rot = 1;
 }

 pillx = pilly / 5;

 if(looper == (CAP-(LEN/2) - MOVE_LEN)) {
  switch(pill_count) {
   case 0:
    current_msg = pillz_msg1;
    break;
   case 1:
    current_msg = pillz_msg2;
    break;
   case 2:
    current_msg = pillz_msg3;
    break;
   case 3:
    current_msg = pillz_msg4;
    break;

  } 
 }

 if(looper == (CAP-(MOVE_LEN/2)))
  current_msg = (char *)0; 

 if(sprite_ram_mutex == 0) {
  reset_sprites();
  if((looper > CAP-(LEN/2)-MOVE_LEN) &&
     (looper < CAP-(MOVE_LEN/2)))
   put_sprite(92+move_down+ pillx,74 + (-bot)+pilly, 3,3, 0,0, 1, 0, 
    (pill_rot == 0 ? 0x1af : 0x1c0));
  
  if(current_msg != (char *)0) {
   l = strlen(current_msg);
   for(i=0;i<l;i++) {
#define Y \
    (cos_tab[((i*30)+(looper2<<4))&0xff] * (float2fix(10.0f)>>8)>>8)
    put_sprite(160-(l*9)+(i*16),146 + Y, 1,1, 0,0, 2, 0, 

      0x1d0 + (current_msg[i]-'a')*4);
   }  
  }
  sprite_ram_mutex = 1;
 }

 if(pill_count == 3)
  if(looper > (CAP-(MOVE_LEN/2)))
   looper_stop = 1;

 if(looper_stop == 0) looper++;
 looper2++;
}
