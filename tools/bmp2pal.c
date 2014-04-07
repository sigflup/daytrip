#include <stdio.h>
#include <SDL.h>

SDL_Surface *bmp;

int main(int argc, char **argv) {
 int i;
 int r,g,b;
 if(argc<2) {
  printf("usage:\n\tbmp2pal <bmp file>\n");
 }

 if((bmp = SDL_LoadBMP(argv[1]))<=0) {
  printf("cannot load %s\n", argv[1]);
  exit(0);
 }

 if(bmp->format->BytesPerPixel != 1) {
  printf("only indexed bmps\n");
  exit(0);
 }

 if(bmp->format->palette->ncolors >15) {
  printf("only up to 15 colors\n");
  exit(0);
 }

 printf("color_t color[] = {\n"
       " { 0,0,0 },\n");
 
 for(i=0;i<bmp->format->palette->ncolors;i++) {
  r = bmp->format->palette->colors[i].r / (0xff/7);
  r*=(0xff/7); 

  g = bmp->format->palette->colors[i].g / (0xff/7);
  g*=(0xff/7); 

  b = bmp->format->palette->colors[i].b / (0xff/7);
  b*=(0xff/7); 
 
  printf(" { %d,%d,%d },\n", r,g,b);
 }
 printf("};\n");
}
