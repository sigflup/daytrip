
typedef struct {
 u8 start, end;
} raster_t;

typedef struct {
 s16 x,y,z;
 s16 u,v;
} vert_t;

typedef struct {
 int v1, v2, v3;
} tri_t;

typedef struct {
 s16 y, x2, y2;
 int stamp;
} line_t;

extern line_t line_buf[64];
extern raster_t raster[128];

extern int stamp;
extern s16 op1, op2, op3, op4, result, result2, result3, result4;
extern s16 cos_op, sin_op;

extern int vert_count;
extern vert_t vert_rot[400];

extern vert_t *current_object;
extern s16 x_scale, y_scale;
extern s16 x_shift, y_shift, z_shift;
extern s16 pitch, yaw;

void rot_verts(s16 in_pitch, s16 in_yaw);
void line(u8 *buf, u16 x1, u16 y1, u16 x2, u16 y2, u8 c);
void wireframe_triangle(u8 *buf, tri_t *tri, u8 c);
