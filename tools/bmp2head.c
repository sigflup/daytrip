#include <stdio.h>
#include <SDL.h>

SDL_Surface *bmp;

int main(int argc, char **argv) {
 int i;
 int x,y;
 unsigned char *pix;
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

 printf("// %dx%d\n", bmp->w, bmp->h);
 printf("const u8 pat[] = {\n");

 pix = (unsigned char *)bmp->pixels;
 for(y=0;y<bmp->h;y++)
  for(x=0;x<bmp->h;x++) {
   printf(" 0x%x,\n", pix[x+(y*bmp->pitch)]+1); 
  }
 printf("};\n");
}
