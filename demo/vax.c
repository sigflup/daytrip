

/* ye-old vax code 2009 sigflup */

#include <libmega.h>
#include "main.h"

#include "vax.h"

#include "ctrl.h"

#include "../mega/vax_data.h"
#include "../mega/vax_map.h"
#include "../mega/vax_col.h"
#include "../mega/vax_sprite.h"

#define	GROUND (120<<8)
#define GRAVITY 0x40	

#define UPSCORE(X) \
 score+=X; \
 print_score(score);


int poll_tick_pos; 
int poll_counter;
u8 lctrl;


u8 flip;
u8 show;

int backpos, score;
int lbackpos;

int keen_x, keen_y;
int vect_x, vect_y;

int strip_buf[64];

int vax_timer;
int vax_song_pos;

u8 keen_still, keen_pose, keen_look;

sprite_param_t tmp_parm;
sprite_param_t duck_parm;

duck_t ducks[DUCKS];

void vax_bottom(void) {
 int x, y;
 tmp_parm.index = 0;
 tmp_parm.h_flip = keen_look;
 tmp_parm.pattern_index = 0x238 + (16 * keen_pose);
 tmp_parm.w = (keen_pose == 7 ? 1 : 3);
 tmp_parm.x = ((keen_x>>8)+128)-backpos;;
 tmp_parm.y = (keen_y>>8)+128;

 set_sprite(&tmp_parm);

  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);
 
 if((backpos>>3)!=lbackpos) {
  for(y=0;y<41;y++) {
   put_strip( ((backpos>>3) +y) % 64, ((backpos>>3) + y) % 256);


  }
 }
 lbackpos = backpos>>3;
}

int kpow(int a) {
 int ret, i;
 ret  = 1;
 for(i=0;i<a;i++)
  ret*=10;
 return ret;
}

void print_score(int num) {
 int j,i,k,n,t,c;
 k = 0;
 t = 0;
 c = 0;
 n = num;
 for(i=0;;) {
  j = n%10;
  t+= (j * kpow(k++));
  VRAM_WRITE(0x8000 + (((64*5) + (10-i))<<1), 0x2000 | ZERO + j);
  i++;
  if(t == num) break;
  n/=10;
 }
 for(;i<9;i++) 
  VRAM_WRITE(0x8000 + (((64*5) + (10-i))<<1), 0x2000 | ZERO - 1);

}

int put_strip(int dst, int src) {
 int y;
 if(strip_buf[dst] == src) return;
 strip_buf[dst] = src;
 for(y=5;y<23;y++) {
  VRAM_WRITE(0x8000+ ((dst + ((y+4)*64))<<1), 0x2000 | vax_map[ ((src)&0xff) + (y*256)]); 
 }
}

void move_ducks(void) {
 int map_x, i;
 int store;
 for( i = 0;i<DUCKS;i++) {
  if(ducks[i].state == COOL) {
   map_x = (ducks[i].x+ (ducks[i].direction == 1 ? 64 : 0))>>3;
   if(map_x > 2048)
    store = 33;
   else
    store = vax_col[map_x + (18*256) ];
   if(store !=0) 
    ducks[i].direction *= -1;
   ducks[i].x+=ducks[i].direction;
  }
 }
}

void move_keen(void) {
 int map_x, map_y;
 int store, collide;
 int i;

 if((vect_x>>8)<0) keen_look = 1;
 if((vect_x>>8)>0) keen_look = 0;

 collide = 0;
 if(vect_y>0) {

  map_x = keen_x>>11;
  map_y = (((keen_y+vect_y)-GROUND)>>11)+18;
  store = vax_col[map_x + (map_y*256) ];
  if(store !=0)
   collide = 1;
  else {
   if( (keen_y+vect_y> GROUND)) 
    collide = 1;
  }

  for(i=0;i<DUCKS;i++) {
   if( ((keen_x>>8) > ducks[i].x) && ((keen_x>>8) < (ducks[i].x+64)) &&
       (((keen_y>>8)+(vect_y>>8))>90) ) {
    ducks[i].state = SQUISH;
    vect_y = -(6<<8);
    UPSCORE(1337);
    collide = 0;
    break;
   }
  }

  if(collide == 1) {
   vect_y = 0;
   keen_still = 1;
  }
 }

 keen_x += vect_x;
 keen_y += vect_y;


 vect_y+=GRAVITY;
 vect_x/=2;


 if(keen_still == 0)  {
  if(vect_y<0) keen_pose = 2;
  if((vect_y>>8)==0) keen_pose = 1;
  if(vect_y>0) keen_pose = 0;
 } else {
  if((vect_x>>4) == 0) 
   keen_pose = 7;
  else 
   keen_pose = 6 - ((keen_x>>12)&3);
 
 }

 if(keen_x < 18<<8)
  keen_x  = 18<<8;
 if(keen_x > ((2048-160)<<8))
  keen_x = (2048-160)<<8;

// print_score(keen_x>>8);

}



void init_vax(void) {
 int i;
 int x,y;
 int store;
 int duck_on_screen;
 v_hook = no_routine;
 h_hook = no_routine;


 //sdfsd

 VDP_SET_REGISTER(NAME_TABLE_SCROLLA_BASE, SCROLL_A_ADDR(0x8000));
 VDP_SET_REGISTER(NAME_TABLE_WINDOW_BASE,  SCROLL_A_ADDR(0x8000));
 VDP_SET_REGISTER(NAME_TABLE_SCROLLB_BASE, SCROLL_B_ADDR(0xc000));
 VDP_SET_REGISTER(SPRITE_ATTRIBUTE_BASE,   SPRITE_ADDR(0xf800));
 VDP_SET_REGISTER(SCROLL_SIZE, SCROLL_WIDTH_64 | SCROLL_HEIGHT_64);


 current_pal = vax_colors;
 current_pal_count = 64;


 show = 0;
 backpos = 8;
 score = 0;

 keen_x = 28<<8;
 keen_y = GROUND;
 vect_x = 0;
 vect_y = 0;
 keen_still = 0;
 keen_pose = 0;
 keen_look = 0;

 ducks[0].x = 369;
 ducks[0].direction = 1;
 ducks[0].state = COOL;

 ducks[1].x = 915;
 ducks[1].direction = -1;
 ducks[1].state = COOL;

 ducks[2].x = 2000;
 ducks[2].direction = -1;
 ducks[2].state = COOL;

 ducks[3].x = 1533;
 ducks[3].direction = 1;
 ducks[3].state = COOL;


 vram_write_rle(0, vax_pat, 0x4700);
 vram_write_rle(0x4700, vax_sprites, 0x1f00);
 vram_write_rle(0xc000, vax_scrollb, 0x2000);
 vram_write_rle(0x8d80, vax_scrolla, 0x280);
 VSRAM_WRITE(0, 0x1e);
 VSRAM_WRITE(2, 0x400- 0x1e);

 for(i=0;i<224*2;i++)
  hscroll_buf[i] = 0;

// VDP_SET_REGISTER(AUTO_INCREMENT, 2);
 for(y=0;y<5;y++)
  for(x=0;x<64;x++) {
   VRAM_WRITE(0x8000 + ((x+ ((y+4)*64))<<1), 0x2000|  vax_map[ (x&0xff) + (y*256)] );   
  }
 print_score(0);

 for(i=0;i<64;i++)
  put_strip(i,i);

 tmp_parm.base = 0xf800;
 tmp_parm.index = 0;
 tmp_parm.w = 3;
 tmp_parm.h = 3;
 tmp_parm.link = 1;
 tmp_parm.priority = 0;
 tmp_parm.palette = 3;
 tmp_parm.v_flip = 0;
 tmp_parm.h_flip = 0;
 tmp_parm.pattern_index = 0x238;
 
 tmp_parm.x = 0;
 tmp_parm.y = 0;

 set_sprite(&tmp_parm);

 duck_parm.base = 0xf800;
 duck_parm.index = 1;
 duck_parm.w = 3;
 duck_parm.h = 3;
 duck_parm.link = 1;
 duck_parm.priority = 0;
 duck_parm.palette = 3;
 duck_parm.v_flip = 0;

 duck_parm.x = 0;
 duck_parm.y = 0;

 set_sprite(&duck_parm);

 vax_timer = 0;
 vax_song_pos = 0;

 buffer[0] = 'b';
 buffer[1] = 'e';
 buffer[2] = 'e';
 buffer[3] = 'f';
 buffer[4] = 'h';
 buffer[5] = 'e';
 buffer[6] = 'a';
 buffer[7] = 'r';
 buffer[8] = 't';

 poll_tick_pos = 0;
 poll_counter =0;
 lctrl = 0;
 ctrl1 = 0;

 v_hook = vax_bottom; 

 fade_to = 0;
 
}


void tick_vax(void) {
 int i;
 int x,y;
 int store;
 int duck_on_screen;

  vax_timer++;


 if((flip&1) == 0) {
/*  lctrl = ctrl1;
  ctrl1 = poll_ctrl1();
  if(lctrl != ctrl1) {
   buffer[ 9 + (poll_tick_pos * 3)] =   ctrl1;
   buffer[ 9 + (poll_tick_pos * 3)+1] = poll_counter &0xff; 
   buffer[ 9 + (poll_tick_pos * 3)+2] = (poll_counter>>8)&0xff;
   poll_tick_pos++;
  }
  poll_counter++;
*/

  if((ctrl_data[(poll_tick_pos *3) +1] == poll_counter &0xff) &&
     (ctrl_data[(poll_tick_pos *3) +2] == (poll_counter>>8)&0xff)) {
   ctrl1 = ctrl_data[poll_tick_pos * 3];
   poll_tick_pos++;
  }
  poll_counter++;
 }
 flip^=1;


//  tmp_parm.pattern_index =0x238 + (16 * (3+((backpos>>3)&3)));
//  set_sprite(&tmp_parm);
  if((ctrl1&RIGHT) == RIGHT) {
   vect_x = (2<<8);
  }
  if((ctrl1&LEFT) == LEFT) {
   vect_x = -(2<<8);
  }

  if((ctrl1&(A|B|C))!=0 )
   if( keen_still == 1) {
    vect_y = -(6<<8);
    keen_still = 0;
    if((keen_y>>8) == 55) {
     // 100 points for jumpin' off a vt100, word
     UPSCORE(100);
    }
   }

/*
  if((ctrl1&RIGHT) == RIGHT) keen_x+=0x100;
  if((ctrl1&LEFT) == LEFT) keen_x-= 0x100;

  if((ctrl1&UP) == UP) keen_y-= 0x100;
  if((ctrl1&DOWN) == DOWN) keen_y+= 0x100;
*/
  if(((keen_x>>8)-backpos) > 160)
   backpos+= (((keen_x>>8)-backpos) - 160);
  if(((keen_x>>8)-backpos) < 10) 
   backpos-= (10 - ((keen_x>>8)-backpos));

  move_keen();
  move_ducks();
  duck_on_screen = 0;
  for(i = 0;i<DUCKS;i++) {
   if(((ducks[i].x+64) > backpos) && (ducks[i].x < (backpos + 320))) {
    if(ducks[i].state == COOL) 
     store = 0x2b0 + (0x20*((ducks[i].x>>4)&1));
    else {
     store = 0x2b0 + 0x40 + (0x20*((vax_timer>>4)&1));
    }
    duck_parm.index = 1;
    duck_parm.w = 3;
    duck_parm.y = 128+120;
    duck_parm.x = 128+(ducks[i].x - backpos);
    if(ducks[i].direction == 1) {
     duck_parm.h_flip = 0;
     duck_parm.pattern_index = store;
    } else {
     duck_parm.h_flip = 1;
     duck_parm.pattern_index = store + 16;
    }
    duck_parm.link = 2;
    set_sprite(&duck_parm);
 
    duck_parm.index = 2;
   if(ducks[i].direction == 1) {
      duck_parm.h_flip = 0;
     duck_parm.pattern_index = store + 16;
    } else {
     duck_parm.h_flip = 1;
     duck_parm.pattern_index = store;
    }
    duck_parm.x+=32;
    set_sprite(&duck_parm);
    duck_on_screen = 1;
    break; 
   } 
  } 
  
  if((duck_parm.y!=0) && (duck_on_screen == 0)) {
   duck_parm.y = 0; 
   duck_parm.index = 1;
   set_sprite(&duck_parm);
   duck_parm.index = 2;
   set_sprite(&duck_parm);
  }
   


  for(i=0;i<224*2;i+=2){
   if(i < (62))
    hscroll_buf[i+1] = 0x400-(backpos>>2);
   else
    hscroll_buf[i+1] = 0x400-(backpos>>1);

   if(i< 80)
    hscroll_buf[i] = 0;
   else {
    if(i < (186*2)) 
     hscroll_buf[i] = 0x400-(backpos);
    else
     hscroll_buf[i] = 0x400-(backpos<<1);
   }
  }
//  show = 1;
} 

