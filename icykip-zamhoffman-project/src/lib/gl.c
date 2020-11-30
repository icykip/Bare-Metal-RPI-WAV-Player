#include "gl.h"
#include "printf.h"
#include "font.h"
#include "strings.h"

void gl_init(unsigned int width, unsigned int height, gl_mode_t mode)
{
    fb_init(width, height, 4, mode);    // use 32-bit depth always for graphics library
}

void gl_swap_buffer(void)
{
   fb_swap_buffer();
}

unsigned int gl_get_width(void)
{
    return fb_get_width();
}

unsigned int gl_get_height(void)
{
    return fb_get_height();
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    return (0xFF << 24) + (r << 16) + (g << 8) + b;
}

void gl_clear(color_t c)
{
    for(int y = 0; y < gl_get_height(); y++){
		for(int x = 0; x < gl_get_width(); x++){
	   		gl_draw_pixel(x, y, c);
		}
    }
}

void gl_draw_pixel(int x, int y, color_t c)
{
    unsigned char *cptr = fb_get_draw_buffer();
    if((x <= gl_get_width()) && (y <= gl_get_height())){
		*(int*)((char*)cptr + y*fb_get_pitch() + x*fb_get_depth()) = c;
    } 
}

color_t gl_read_pixel(int x, int y)
{
     unsigned char *cptr = fb_get_draw_buffer();
     if((x <= gl_get_width()) && (y <= gl_get_height())){
         return *(int*)((char*)cptr + y*fb_get_pitch() + x*fb_get_depth());
     }
     return 0;
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    for(int i = y; i < h+y; i++){
		for(int j = x; j < w+x; j++){
			gl_draw_pixel(j, i, c);
		}
    } 
}

void gl_draw_char(int x, int y, int ch, color_t c)
{
    unsigned char buf[font_get_size()];
    char out = ch;
    if(font_get_char(out, buf, sizeof(buf)) == 0) return;
    unsigned int step = 0;
    for(int i = y; i < y+gl_get_char_height(); i++){
         for(int j = x; j < gl_get_char_width()+x; j++){
 			if(buf[step] == 0xFF) gl_draw_pixel(j, i, c);
				step++;              
         }
    } 
}

void gl_draw_string(int x, int y, const char* str, color_t c)
{
    char ch = *str;
    while(ch != '\0'){
	gl_draw_char(x, y, ch, c);
	str += 1;
	ch = *str; 
	x += gl_get_char_width();
    }
}

unsigned int gl_get_char_height(void)
{
    return font_get_height();
}

unsigned int gl_get_char_width(void)
{
    return font_get_width();
}
