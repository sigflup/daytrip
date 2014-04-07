#include <libmega.h>
#include "vert.h"

#include "../fb_tab.h"

raster_t raster[128];

int stamp;

line_t line_buf[64];

s16 op1, op2, op3, op4, result, result2, result3, result4;
s16 cos_op, sin_op;

int vert_count;
vert_t vert_rot[400];
vert_t *current_object;
s16 x_scale, y_scale;
s16 x_shift, y_shift, z_shift;
s16 pitch,yaw;

void rot_verts(s16 in_pitch, s16 in_yaw) {
 int i;
 s32 ppitch, yyaw;
 ppitch = (in_pitch>>8)&0xff;
 yyaw = (in_yaw>>8)&0xff;
 s16 pcos, psin, ycos, ysin;
 s16 t1, t2;

 s16 x,y,z;

 pcos = cos_tab[ppitch];
 psin = sin_tab[ppitch];
 ycos = cos_tab[yyaw];
 ysin = sin_tab[yyaw];

 for(i=0;i<vert_count;i++) {
 
  x = current_object[i].x<<8;
  y = current_object[i].y<<8;
  z = current_object[i].z<<8;

  t1 = ((x * pcos) + (z * psin))>>8;
  t2 = ((z * pcos) - (x * psin))>>8;

  x = t1;
  z = t2;

  t1 = ((y * ycos) + (z * ysin))>>8;
  t2 = ((z * ycos) - (y * ysin))>>8;

  y = t1;
  z = t2;

  vert_rot[i].z = z;

  z+=z_shift;
  z>>=5;
  x = (x<<8) / z;
  y = (y<<8) / z;

  vert_rot[i].x = x;
  vert_rot[i].y = y;

  vert_rot[i].x = (vert_rot[i].x * x_scale)>>8;
  vert_rot[i].y = (vert_rot[i].y * y_scale)>>8;

  vert_rot[i].x>>=8;
  vert_rot[i].y>>=8;

  vert_rot[i].x+=x_shift;
  vert_rot[i].y+=y_shift;

 }
}




void line(u8 *buf, u16 x1, u16 y1, u16 x2, u16 y2, u8 c) {
 int ylonger;
 s16 incval, endval;
 s16 shortlen;
 s16 longlen;
 s16 swap;
 s16 decinc;
 s16 i, j;

/* if(line_buf[x1].stamp == stamp &&
    line_buf[x1].y == y1 &&
    line_buf[x1].y2 ==y2 &&
    line_buf[x1].x2 ==x2) return; */

 line_buf[x1].y = y1;
 line_buf[x1].x2 = x2;
 line_buf[x1].y2 = y2;
 line_buf[x1].stamp = stamp;


 ylonger = 0;
 shortlen = y2 - y1;
 longlen =  x2 - x1;

 if(abs(shortlen) > abs(longlen)) {
  swap = shortlen;
  shortlen = longlen;
  longlen = swap;
  ylonger = 1;
 } 
 endval = longlen;
 if(longlen<0) {
  incval = -1;
  longlen = -longlen;
 } else
  incval = 1;

 if(longlen == 0)
  decinc=0;
 else
  decinc = (shortlen<<8) / longlen;


 if(ylonger == 1) {
  for(i=0, j=0;i!=endval;i+=incval, j+=decinc) 
   buf[fb_tab[(x1 + (j>>8)) + ((y1+i) * 64)]] = c;
 } else {
  for(i=0, j=0;i!=endval;i+=incval, j+=decinc) 
   buf[fb_tab[(x1+i)+ ((y1 + (j>>8))* 64)]] = c;
 }
}

void wireframe_triangle(u8 *buf, tri_t *tri, u8 c) {
 line(buf, vert_rot[tri->v1].x, vert_rot[tri->v1].y,
           vert_rot[tri->v2].x, vert_rot[tri->v2].y, c);
 line(buf, vert_rot[tri->v2].x, vert_rot[tri->v2].y,
           vert_rot[tri->v3].x, vert_rot[tri->v3].y, c);
 line(buf, vert_rot[tri->v3].x, vert_rot[tri->v3].y,
           vert_rot[tri->v1].x, vert_rot[tri->v1].y, c);

}

/* line starts/ends {{{ */
void line_starts(u16 x1, u16 y1, u16 x2, u16 y2) {
 int ylonger;
 s16 incval, endval;
 s16 shortlen;
 s16 longlen;
 s16 swap;
 s16 decinc;
 s16 i, j;

 ylonger = 0;
 shortlen = y2 - y1;
 longlen =  x2 - x1;

 if(abs(shortlen) > abs(longlen)) {
  swap = shortlen;
  shortlen = longlen;
  longlen = swap;
  ylonger = 1;
 } 
 endval = longlen;
 if(longlen<0) {
  incval = -1;
  longlen = -longlen;
 } else
  incval = 1;

 if(longlen == 0)
  decinc=0;
 else
  decinc = (shortlen<<8) / longlen;


 if(ylonger == 1) {
  for(i=0, j=0;i!=endval;i+=incval, j+=decinc)
   raster[y1+i].start = x1 + (j>>8);
 } else {
  for(i=0, j=0;i!=endval;i+=incval, j+=decinc) 
   raster[y1+(j>>8)].start = x1+i;
 }
}

void line_ends(u16 x1, u16 y1, u16 x2, u16 y2) {
 int ylonger;
 s16 incval, endval;
 s16 shortlen;
 s16 longlen;
 s16 swap;
 s16 decinc;
 s16 i, j;

 ylonger = 0;
 shortlen = y2 - y1;
 longlen =  x2 - x1;

 if(abs(shortlen) > abs(longlen)) {
  swap = shortlen;
  shortlen = longlen;
  longlen = swap;
  ylonger = 1;
 } 
 endval = longlen;
 if(longlen<0) {
  incval = -1;
  longlen = -longlen;
 } else
  incval = 1;

 if(longlen == 0)
  decinc=0;
 else
  decinc = (shortlen<<8) / longlen;


 if(ylonger == 1) {
  for(i=0, j=0;i!=endval;i+=incval, j+=decinc)
   raster[y1+i].end = x1 + (j>>8);
 } else {
  for(i=0, j=0;i!=endval;i+=incval, j+=decinc) 
   raster[y1+(j>>8)].end = x1+i;
 }
}

/* }}} */

void flat_triangle(u8 *buf, tri_t *tri, u8 c) {
 u8 start, end, middle;
 int i;
 u8 j;
 vert_t *vtop, *vmiddle, *vbottom;

#define V1	vert_rot[tri->v1]
#define V2	vert_rot[tri->v2]
#define V3	vert_rot[tri->v3]

 if((V1.y == V2.y) && (V1.y==V3.y)) return;

 if((V1.y <= V2.y) && (V1.y <= V3.y)) {
  vtop = &V1;
  if(V2.y <= V3.y) {
   vmiddle = &V2;
   middle = V2.y;
   vbottom = &V3;
  } else {
   vmiddle = &V3;
   middle = V3.y;
   vbottom = &V2;
  }
 }

 if((V2.y <= V1.y) && (V2.y <= V3.y)) {
  vtop = &V2;
  if(V1.y <= V3.y) {
   vmiddle = &V1;
   middle = V1.y;
   vbottom = &V3;
  } else {
   vmiddle = &V3;
   middle = V3.y;
   vbottom = &V1;
  }
 }

 if((V3.y <= V1.y) && (V3.y <= V2.y)) {
  vtop = &V3;
  if(V1.y <= V2.y) {
   vmiddle = &V1;
   middle= V1.y;
   vbottom = &V2;
  } else {
   vmiddle = &V2;
   middle = V2.y;
   vbottom = &V1;
  }
 }

 start = vtop->y;
 end = vbottom->y;

 line_starts(vtop->x, vtop->y, vbottom->x, vbottom->y);
 line_ends(vtop->x, vtop->y, vmiddle->x, vmiddle->y);
 line_ends(vmiddle->x,vmiddle->y, vbottom->x, vbottom->y);

 /*
 if(raster[middle].start <= raster[middle].end) {
  for(i=start;i<end;i++)
   for(j= raster[i].start;j<raster[i].end;j++)
    buf[fb_tab[ j+(i*64)]] = c;
 } else {
  for(i=start;i<end;i++)
   for(j= raster[i].end;j<raster[i].start;j++)
    buf[fb_tab[ j+(i*64)]] = c;
 }*/


 for(i=start;i<end;i++) {
  if(raster[i].start <= raster[i].end)
   for(j = raster[i].start;j<raster[i].end;j++)
    buf[fb_tab[ j + (i * 64)]] = c; 
  else
   for(j = raster[i].end;j<raster[i].start;j++)
    buf[fb_tab[ j + (i * 64)]] = c; 
 }
}
