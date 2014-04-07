#define COOL	0	
#define SQUISH	1

#define	ZERO	0x21d

#define DUCKS	4

typedef struct {
 int x; 
 s8 direction; 
 u8 state;
} duck_t;


extern duck_t ducks[DUCKS];

void vax(void);
