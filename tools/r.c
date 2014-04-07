#include <stdio.h>
#include <SDL.h>

#define WIDTH 512 
#define HEIGHT 512 

int map[WIDTH * HEIGHT];

void box(int x1, int y1, int x2, int y2, int c) {
 int x, y;
 for(y=0;y< (y2-y1);y++) 
  for(x=0;x< (x2-x1);x++) 
   map[ (x+x1) + ((y+y1)*WIDTH)] = c;
  
}

int main(void) {
 int i;
 for(i=0;i<(WIDTH * HEIGHT);i++) 
  map[i] = 0;

 box(50, 50, 100, 100, 1); 

 printf("const u8 test2_map[] = { \n");
 for(i=0;i<(WIDTH * HEIGHT);i++)
  printf("%d,\n", map[i]);
 printf("};\n");
}
