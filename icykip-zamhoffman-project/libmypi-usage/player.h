#ifndef PLAYER_H
#define PLAYER_H
/*
* Sets all the required pins to output(pins for inverter and DAC)
*
*/
void player_init(void);

/*
* Using the FATFS library this function reads a wav file into a signed short array; 
*
* @param song_name - the name of the file you want read in as an array
*
*/
signed short * load_song(char * song_name);

/*
* Given a song name play calls load song, reads through the resulting
* array converting each number from 16 bits to 8 bits and then turning
* on the corisponding pins on the DAC and the inveereter to create a 
* readable analog signal to be passed to the amplifier and speakers.
*
* Includes button interaction for pause, play, restart, next and
* exit(clear queue).
*
* @param song_name - name of the song you want to play
*
*/
unsigned int play(char * song_name);

#endif
