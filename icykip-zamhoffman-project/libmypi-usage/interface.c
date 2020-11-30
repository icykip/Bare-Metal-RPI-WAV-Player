#include "gpio.h"
#include "gpio_ref.h"
#include "gl.h"
#include "gl_ref.h"
#include "strings.h"
#include "interface.h"
#include "menu.h"
#include "printf.h"
#include "player.h"
#include <stdarg.h>

static unsigned int char_width; //width of char
static unsigned int char_height; //height of char
static unsigned int frame_width; //width of frame
static unsigned int frame_height; //height of frame

void interface_init(unsigned int nrows, unsigned int ncols){
    char_width = gl_get_char_width();
    char_height = gl_get_char_height();
    frame_height = nrows;
    frame_width = ncols;
    gl_init(ncols * char_width, nrows * char_height, GL_DOUBLEBUFFER);
    gl_clear(GL_BLACK);
    gl_swap_buffer();
    gl_clear(GL_BLACK);
}
 
void interface_clear(void){
    gl_clear(GL_BLACK);
    gl_swap_buffer();
    gl_clear(GL_BLACK);
}

void queue_indicator(unsigned int start_x, unsigned int start_y){										/* THIS FUNCTION IS BROKEN */
	gl_draw_rect(start_x + 20, (start_y * char_height) + 20, 40, 40, GL_BLACK);
	gl_swap_buffer();
	gl_draw_rect(start_x + 20, (start_y * char_height) + 20, 40, 40, GL_BLACK);
}

unsigned int * display_current(char * song, unsigned int duration, unsigned int state){
	interface_clear();
	unsigned int * player_start = song_box(0, 0, frame_width, frame_height, song);
	draw_player_features(player_start[0], player_start[1], duration, state);
	gl_swap_buffer();																					/* Draws outer box with song namee and play skip and pause symbols */					
	song_box(0, 0, frame_width, frame_height, song);
    draw_player_features(player_start[0], player_start[1], duration, state);
	return player_start;
}


unsigned int display_songs(char ** songs, unsigned int num_songs){
	interface_clear();
	unsigned int height = frame_height/num_songs;
	for(int i = 0; i < num_songs; i++){																	/* Displays all songs in a list using boxes with the name of the song inside */
		song_box(0, (i*height*char_height), height, frame_width, songs[i]);
	}
	return height;
}

unsigned int * song_box(unsigned int start_x, unsigned int start_y, unsigned int rows, unsigned int cols, char * name){ 
	rows *= char_width;
	cols *= char_width;
    gl_draw_rect(start_x, start_y, cols, 20, GL_AMBER);
    gl_draw_rect(start_x, start_y + rows, cols, 20, GL_AMBER);
    gl_draw_rect(start_x, start_y, 20, rows, GL_AMBER);
    gl_draw_rect(start_x + cols - 20, start_y, 20, rows, GL_AMBER);
    																									/* Draws a box with the name of the song in the middle */ 
	start_y += (rows/2 - (char_height)/2);
	start_x = (start_x + cols/2) - ((strlen(name) * char_width)/ 2);
	unsigned int player_start[2] = {start_x, start_y + char_height * 2};
	gl_draw_string(start_x, start_y, name, GL_AMBER);
	return player_start;
}

void toggle_box_colors(unsigned int start_x, unsigned int start_y, unsigned int rows, unsigned int cols){
	for(int y = start_y; y < start_y + (rows * char_height) - 20; y++){
		for(int x = start_x; x < start_x + (cols * char_width); x++){									/* Invert the color of a box to signify it is selected */
			if(gl_read_pixel(x, y) == GL_AMBER) gl_draw_pixel(x, y, GL_BLACK);
			else gl_draw_pixel(x, y, GL_AMBER);
		}
	}
}

void select_song(unsigned int x, unsigned int height){
    toggle_box_colors(0, x * height * char_height, height, frame_width);
    gl_swap_buffer();																					/* calls toggle box for both buffer */ 
    toggle_box_colors(0, x * height * char_height, height, frame_width);
}

void draw_player_features(unsigned int start_x, unsigned int start_y, double duration, unsigned int pa_or_pl){
	gl_draw_rect(start_x, start_y + 5, duration, 5, GL_AMBER);
	draw_triangle(start_x, start_y + 20);
	draw_triangle(start_x + 20, start_y + 20);
	gl_draw_rect(start_x + 80, start_y, 25, 40, GL_BLACK);
	if(pa_or_pl == 1){
		gl_draw_rect(start_x + 80, start_y, 15, 40, GL_AMBER);
		gl_draw_rect(start_x + 105, start_y, 15, 40, GL_AMBER);
	}																									/* Draws play pause and skip symbols */
	else{
		rev_draw_triangle(start_x + 90, start_y + 20); 
	}
	rev_draw_triangle(start_x + 160, start_y + 20);
	rev_draw_triangle(start_x + 180, start_y + 20);
}	


void draw_triangle(unsigned int start_x, unsigned int start_y){
	for(int x = 0; x < 20; x++){
		gl_draw_rect(start_x + x, start_y - x, 1, x*2 + 1, GL_AMBER);
	}
}

void rev_draw_triangle(unsigned int start_x, unsigned int start_y){
	for(int x = 0; x < 20; x++){
		gl_draw_rect(start_x + x, start_y - (20-x), 1, (20-x) * 2 + 1, GL_AMBER);
	}
}

