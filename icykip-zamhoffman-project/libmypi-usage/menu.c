#include "ff.h"
#include "string.h"
#include "malloc.h"
#include "pi.h"
#include "gpio.h"
#include "gl.h"
#include "gpio_ref.h"
#include "player.h"
#include "printf.h"
#include "interface.h"
#include "timer.h"

static unsigned int num_songs = 0;
static const int button_pins[4] = {GPIO_PIN27, GPIO_PIN22, GPIO_PIN23, GPIO_PIN24};

void scan_songs(char ** songs){

    FATFS fs;
    FRESULT res;															/* Mount drive in order ot read files */
    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        printf("Mount Failed");
		return songs;
	}

	DIR dir;
	static FILINFO fno;

	res = f_opendir(&dir, "Music");			
	if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
			if(fno.fname[0] >= 'A' && fno.fname[0] <= 'Z'){
				memcpy(songs[num_songs], fno.fname, strlen(fno.fname));		/* Read every file that starts with a valid Alphabetic char that is in the music folder */
				num_songs += 1;
			}
        }
        f_closedir(&dir);	
    }
}

void run_music_player(void){
	char ** queue = (char **)malloc(10 * sizeof(char *));
	for(int i = 0; i < 10; i++){
		queue[i] = (char*)malloc(50 * sizeof(char));						/* Initialize queue */
	}
	unsigned int queue_pointer = 0;
	unsigned int queue_length = 0;
	char ** songs = (char **)malloc(10 * sizeof(char *));
	for(int i = 0; i < 10; i++){
		songs[i] = (char *)malloc(50 * sizeof(char));						/* Initialize array of playable songs in Music folder */
	}	
	scan_songs(songs);
	unsigned int height = display_songs(songs, num_songs);
	gl_swap_buffer();
	display_songs(songs, num_songs);
	unsigned int song_pointer = 0;
	unsigned int in_queue = 0;												/* set interface and prep the player library */
	player_init();
	select_song(song_pointer, height);
	while(1){
		if(!gpio_read(button_pins[0])){
			timer_delay(1);
			if(song_pointer != 0){
				select_song(song_pointer, height);
				song_pointer -= 1;											/* scroll down one song (sets interface and song_pointer) */
				select_song(song_pointer, height);
				printf("song: %s\n", songs[song_pointer]);
			}
		}
		else if(!gpio_read(button_pins[1])){
			timer_delay(1);
			if(queue_length != 10){
				in_queue = 0;
				for(int i = 0; i < queue_length; i++){						/* Adds current song to queue if not already in the queue*/
					if(strcmp(queue[i],songs[song_pointer]) == 0){
						in_queue = 1;
					}
				}
				if(in_queue == 0){
					memcpy(queue[queue_length], songs[song_pointer], strlen(songs[song_pointer]));
					printf("song: %s added to queue\n", songs[song_pointer]);
					queue_length++;
				}
			}
		}
		else if(!gpio_read(button_pins[2])){
			timer_delay(1);
			unsigned int cont = 0;
			printf("play song: %s\n", songs[song_pointer]);					/* plays the current song and then sets the queue to be played immeadiatly after */
			cont = play(songs[song_pointer]);
			if(cont == 0)break;
			while(queue_pointer != queue_length){
				cont = play(queue[queue_pointer]);
				queue_pointer++;
				if(cont == 0) break;
			}
			queue_length = 0;
			queue_pointer = 0;
			display_songs(songs, num_songs);
			gl_swap_buffer();
			display_songs(songs, num_songs);
			select_song(song_pointer, height);
			timer_delay(1);
		}
		else if(!gpio_read(button_pins[3])){
			timer_delay(1);
			if(song_pointer != num_songs){
				select_song(song_pointer, height);							/* scroll up one song */
				song_pointer += 1;
				select_song(song_pointer, height);
				printf("song: %s\n", songs[song_pointer]);
			}
		}
	}
}
