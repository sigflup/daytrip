#define SPRITE_BASE	0xd400

extern int updating_palette;
extern int pal_mutex;
extern int dma_corrupt;

extern int brightness, last_brightness;

extern u8 buffer[0x2000];
extern u8 buffer2[0x2000];

extern u16 hscroll_buf[244*2];
extern int brightness, fade_to;
extern int updating_palette;

extern color_t *current_pal;
extern int current_pal_count;

void update_vgm_pos(void);
void update_palette(void);

s16 rand_short(s32 *seed);

extern void (*v)(void);
