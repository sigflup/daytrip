


typedef struct {
 int x, y;
 int w,h;
 int v_flip, h_flip;
 int pal, prio, index;
} prev_sprite_t;

extern int sprite_index;

extern prev_sprite_t prev_sprite;

extern u16 sprite_ram[80*4];

void clear_sprites(void);
void reset_sprites(void);

void put_sprite(int x, int y, int w, int h, int v_flip, int h_flip, int pal, 
  int prio, int index);
