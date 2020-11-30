#include "console.h"
#include "strings.h"
#include "gl.h"
#include "shell.h"
#include "printf.h"
#include "malloc.h"
#include <stdarg.h>

static void process_char(char ch);
static char ** content;
static unsigned int cursor[2];
static unsigned int console_end = 0; 
static unsigned int ysize;
static unsigned int xsize;
static color_t background;

void console_init(unsigned int nrows, unsigned int ncols)
{
    // TODO: implement this function
    // call `process_char` to silence the compiler's warning
    // that it's defined but not used
    // once you use `process_char` elsewhere, you can delete
    // this line of code
    gl_init(ncols*gl_get_char_width(), nrows*gl_get_char_height(), GL_DOUBLEBUFFER);
    background = gl_color(0x0, 0x0, 0x0);
    gl_clear(background);
    gl_swap_buffer();
    gl_clear(background);
    content = (char**)malloc(sizeof(char*) * nrows);
    for(int y = 0; y < nrows; y++){
	content[y] = (char*)malloc(ncols);
    }
    ysize = nrows;
    xsize = ncols;
    cursor[0] = 0;
    cursor[1] = 0;
    console_clear();
}

void console_clear(void)
{
    for(int y = 0; y < ysize; y++){
	for(int x  = 0; x < xsize; x++){
		*( = ' ';
	}
    }	
    cursor[0] = 0;
    cursor[1] = 0;
    console_end = 0;
    gl_clear(background);
    gl_swap_buffer();
    gl_clear(background);
}

int console_printf(const char *format, ...)
{
    unsigned int output = 0;
    char * buf = malloc(xsize * ysize);
    va_list args;
    va_start(args, format);
    output = vsnprintf(buf, ysize * xsize - console_end, format, args);
    while(*buf != '\0'){
	process_char(*buf);
	buf += 1;
	printf("%s", content);
    }
    return output;
}

void vert_scroll(){
    printf("%s", content);
    int height = gl_get_char_height();
    int width = gl_get_char_width();
    for(int y = 0; y < ysize; y++){
        for(int x  = 0; x < xsize; x++){
		if(y == ysize-1){
			*(content + y*ysize + x) = ' ';
			 gl_draw_rect(width*x, height*y, width, height, background);
		}
		else{
			*(content + (y)*ysize + x) = *(content + (y+1)*ysize + x);
			gl_draw_rect(width*x, height*y, width, height, background);
			gl_draw_char(width*x, height*y, *(content + y*ysize +  x), GL_AMBER);
		}
	}
    }
    gl_swap_buffer();
    for(int y = 0; y < ysize; y++){
         for(int x  = 0; x < xsize; x++){
                 if(y == ysize-1){ 
                          gl_draw_rect(width*x, height*y, width, height, background);
                 }
                 else{   
                         gl_draw_rect(width*x, height*y, width, height, background);
                         gl_draw_char(width*x, height*y, *(content + y*ysize +  x), GL_AMBER);
                 }
         }
     }
}

static void process_char(char ch){
    int w = gl_get_char_width();
    int h = gl_get_char_height();
    if(ch == '\b'){
	if(cursor[1] == 0){
		cursor[1] = xsize;
		cursor[0]--;
	} 
	else cursor[1]--;
	gl_draw_rect(w*cursor[1], h*cursor[0], gl_get_char_width(), gl_get_char_height(), background);
	gl_swap_buffer();
	gl_draw_rect(w*cursor[1], h*cursor[0], gl_get_char_width(), gl_get_char_height(), background);
	*(content + cursor[0]*ysize +  cursor[1]) = ' ';	
    }	
    else if(ch == '\r'){
	cursor[1] = 0;
    }
    else if(ch == '\n' || curosr[1] == xsize){
	if(cursor[0] == ysize-1){
		vert_scroll();
		cursor[1] = 0;
		cursor[0] = ysize-1;
	}
	else{
	     cursor[0]++;
	     cursor[1] = 0;
	}
    }
    else if(ch == '\f'){
	console_clear();
    }
    else{
	if(cursor[0] == ysize){
            vert_scroll();
            cursor[1] = 0;
	    cursor[0]  = ysize-1;
        }
	*(content + cursor[0]*ysize +  cursor[1]) = ch;
	gl_draw_rect(w*cursor[1], h*cursor[0], gl_get_char_width(), gl_get_char_height(), background);
	gl_draw_char(w*cursor[1], h*cursor[0], ch, GL_AMBER);
	gl_swap_buffer();
	gl_draw_rect(w*cursor[1], h*cursor[0], gl_get_char_width(), gl_get_char_height(), background);
	gl_draw_char(w*cursor[1], h*cursor[0], ch, GL_AMBER);
	if(cursor[1] == xsize-1){
			cursor[0]++;
			cursor[1] = 0;
	}
	else cursor[1]++;
    }
}
