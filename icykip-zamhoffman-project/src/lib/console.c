#include "gl.h"
#include "shell.h"
#include "console.h"
#include "printf.h"
#include <stdarg.h>

static void process_char(char ch);
static char ** content; 
static unsigned int char_width; //width of char
static unsigned int char_height; //height of char
static unsigned int frame_width; //width of frame
static unsigned int frame_height; //height of frame
static unsigned int cx = 0; //cursor x pos
static unsigned int cy = 0; //cursor y pos
static color_t background = GL_BLACK;
static color_t color = GL_AMBER;

void console_init(unsigned int nrows, unsigned int ncols){
    char_width = gl_get_char_width();
    char_height = gl_get_char_height();
    frame_height = nrows;
    frame_width = ncols;
    gl_init(ncols * frame_width, nrows * frame_height, GL_DOUBLEBUFFER);
    gl_clear(background);
    gl_swap_buffer();
    gl_clear(background);
    content = (char**)malloc(sizeof(char*) * nrows);
    for(int i = 0; i < nrows; i++){
	content[i] = (char*)malloc(ncols);
    }
    cx = 0;
    cy = 0;
}

void console_clear(void){
    for(int y = 0; y < frame_height; y++){
	for(int x = 0; x < frame_width; x ++){
	    content[y][x] = '\0';
	}
    }
    cx = 0;
    cy = 0;
    gl_clear(background);
    gl_swap_buffer();
    gl_clear(background);
}

int console_printf(const char *format, ...){
    unsigned int output = 0;
    char * buf = malloc(frame_height * frame_width);
    va_list args;
    va_start(args, format);
    output = vsnprintf(buf, frame_height * frame_width, format, args);
    while(*buf != '\0'){
	process_char(*buf);
	buf += 1;
    }
    return output;
}

static void vert_scroll(void){
    for(int y = 0; y < frame_height; y++){
        for(int x = 0; x < frame_width; x ++){
	    gl_draw_rect(char_width * x, char_height * y, char_width, char_height, background);
	    if(y == frame_height-1) content[y][x] = '\0';
	    else{
	   	content[y][x] = content[y+1][x];
		gl_draw_char(char_width * x, char_height * y, content[y][x], color);
	    }
        }
    }
    gl_swap_buffer();
    for(int y = 0; y < frame_height; y++){
        for(int x = 0; x < frame_width; x ++){
            gl_draw_rect(char_width * x, char_height * y, char_width, char_height, background);
            if(y != frame_height-1) gl_draw_char(char_width * x, char_height * y, content[y][x], color);
        }
    }   
    cx = 0;
    cy = frame_height-1; 
}

static void process_char(char ch){
    if(ch == '\b'){
	if(cx == 0){
	    cx = frame_width-1;
	    cy--;
	}
	else cx--;
	gl_draw_rect(char_width * cx, char_height * cy, char_width, char_height, background);
	gl_swap_buffer();
	gl_draw_rect(char_width * cx, char_height * cy, char_width, char_height, background);
	content[cy][cx] = ' ';
    }
    else if(ch == '\r') cx = 0;
    else if(ch == '\n'){
	if(cy == frame_height-1){	
	    vert_scroll();
	}
	else{
	    cx = 0;
	    cy++;
	}		
    }
    else if(ch == '\f') console_clear();
    else{
	if(cx == frame_width) console_printf("\n");
	content[cy][cx] = ch;
	gl_draw_rect(char_width * cx, char_height * cy, char_width, char_height, background);
	gl_draw_char(char_width * cx, char_height * cy, ch, color);
	gl_swap_buffer();
	gl_draw_rect(char_width * cx, char_height * cy, char_width, char_height, background);
	gl_draw_char(char_width * cx, char_height * cy, ch, color);
	cx++;
    }
}
