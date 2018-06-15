#include <config.h>

#define FBDEVICE	"/dev/fb0"
#define WIDTH		1024
#define HEIGHT		600
#define WHITE		0xffffffff
#define BLACK		0x00000000
#define BLUE		0x000000ff
#define GREEN		0x0000ff00
#define RED			0x00ff0000




int fb_open(void);
int fb_close(void);
void fb_draw_back(unsigned int width, unsigned int height, unsigned int color);
void fb_draw_line(unsigned int color);
#if 0
void fb_draw_picture(void);
void fb_draw_picture2(void);
void fb_draw_picture3(void);
void fb_draw_picture4(void);
void fb_draw_picture5(void);
void fb_draw_picture6(unsigned int x0, unsigned int y0);
void fb_draw_picture7(unsigned int x0, unsigned int y0);
void fb_draw_picture8(unsigned int x0, unsigned int y0);
void fb_draw_picture9(unsigned int x0, unsigned int y0);
#endif
void fb_draw(struct pic_info *pPic);
void fb_draw2(struct pic_info *pPic);






