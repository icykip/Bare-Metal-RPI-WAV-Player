#ifndef INTERFACE_H
#define INTERFACE_H

/*
* Initialize the console. The console begins empty and
* nothing is displayed, static variables are filled.
*
* @param nrows square root of the requested number of rows
* @param ncols square root of the requested number of columns
*/

void interface_init(unsigned int nrows, unsigned int ncols);

/*
* Clear all console images.
*/
void interface_clear(void);

/* Creates a small square in the uppper left corner of
* the song selection box to indicate that it is already
* in the queue.
*
* @param start_x the upper left corner of the box 
* @param start_y the upper left corner of the box
*/

void queue_indicator(unsigned int start_x, unsigned int start_y);

/*
* Displays the song that is currenntly playing
* along with the current place in the song and
* whether or not it is paused.
*
* @return the starting x and y for the time bar and
* play/pause button to be updated.
*
* @param song - the name of the song playing
* @param duration - how much of the song has gone by already
* @param state - 1 if the song is playing, 0 if it is paused
*/

unsigned int * display_current(char * song, unsigned int duration, unsigned int state);

/*
* Displays all availible songs for the user to select to either play or queue.
* 
* @return the height of each "song box".
*
* @param songs - list of all the songs that can be played
* @param num_songs - the number of songs in the list
*/

unsigned int display_songs(char ** songs, unsigned int num_songs);

/*
* Creates an open box with the name of a given song placed inside.
*
* returns the starting poistion for the state features(duration and play/pause)
*
* @param start_x - top left corner to begin the box
* @param start_y - top left corner to begin the box
* @param rows - how many rows high should the box be
* @param cols - how many columns wide should the box be
* @param name - name of the song to place in the middle
*
*/

unsigned int * song_box(unsigned int start_x, unsigned int start_y, unsigned int rows, unsigned int cols, char * name);

/*
* Inverts the colors of a given box to give it the appereance of
* having been selected.
* 
* @param start_x - top left corner of box to be inverted
* @param start_y - top left corner of box to be inverted
* @param rows - the number of rows high that the box is
* @param cols - the number of cols wide that the box is
*/

void toggle_box_colors(unsigned int start_x, unsigned int start_y, unsigned int rows, unsigned int cols);

/*
* Toggles song for both frame buffers
*
* @param x - int for the song to be selected
* @param height - distance between each song box
*/
void select_song(unsigned int x, unsigned int height);

/*
* Draws the progress bar as well as the skip play/pause and reestart song buttons
*
* @param start_x - the top left corner of where to begin drawing
* @param start_y - the top left corner of where to begin drawing
* @param duration - what percentage of the song has been completed
* @param pa_or_pl - 1 for playing 0 if paused
*
*/

void draw_player_features(unsigned int start_x, unsigned int start_y, double duration, unsigned int pa_or_pl);


/*
* draws a trianlge with the pointy side facing left
* 
* @param start_x - starting x position for triangle
* @param start_y - starting y position for triangle
*/

void draw_triangle(unsigned int start_x, unsigned int start_y);


/*
* draws a trianlge with the pointy side right
* 
* @param start_x - starting x position for triangle
* @param start_y - starting y position for triangle
*/

void rev_draw_triangle(unsigned int start_x, unsigned int start_y);

#endif
