#include "strings.h"
#include "printf.h"
#include "malloc.h"
#include "gpio.h"
#include "gpio_ref.h"
#include "uart.h"
#include "ff.h"
#include "pi.h"
#include "gl.h"
#include "player.h"
#include "interface.h"
#include "menu.h"
#include "timer.h"
#include <stdint.h>

static const int dac_pins[8] = {GPIO_PIN19, GPIO_PIN13, GPIO_PIN6, GPIO_PIN5, GPIO_PIN1, GPIO_PIN7, GPIO_PIN8, GPIO_PIN25};
static const int inv_pins[4] = {GPIO_PIN20, GPIO_PIN21, GPIO_PIN16, GPIO_PIN12};	
static const int button_pins[4] = {GPIO_PIN27, GPIO_PIN22, GPIO_PIN23, GPIO_PIN24};
static unsigned int song_size = 0;

void player_init(void){
    for(int i = 0; i < 8; i++){
		gpio_set_output(dac_pins[i]);
		if(i < 4){
			gpio_set_output(inv_pins[i]);
		}
    }
}

signed short * load_song(char * song_name){
	FATFS *fs;     																		/* Ponter to the filesystem object */
    fs = malloc(sizeof (FATFS));          												 /* Get work area for the volume */
    f_mount(fs, "", 0);

    FRESULT res;
    FIL * file;
	DIR dir;
	signed char * fail = "\0";
    file = malloc(sizeof (FIL)); 														/* Get work area for the file */
	har * path = "/Music/";        
	unsigned int name_length = strlen(song_name);
	strlcat(path, song_name, 100);  													/* Add /Music/ before song name to access songs in Music folder */
    res = f_open(file, path, FA_READ);
    if (res != FR_OK) {
        printf("Could not find song %s. Error: %s\n", song_name, ff_err_msg(res));
        return fail;
    }
	unsigned int * nread;
    unsigned int size = (int)f_size(file);
    signed short * song = malloc((size - 44) * sizeof(char));
	f_lseek(file, 45);																	 /* skip to byte 45 of the wav file (first 44 are header) */
    res = f_read(file, song, size - 45, nread);											 /* read in the file to the array song */
    if (res != FR_OK || *nread != (size-45)) {
    	printf("Could not read from file %s. Error: %s\n", song_name, ff_err_msg(res));
        return fail;
    }
	song[size - 44] = song[0];
    f_close(file);
	f_closedir(&dir);
	song_size = size-45;
	return song;
}

unsigned int play(char * song_name){
	unsigned int * status_loc = display_current(song_name, 0, 1);						/* Update interface to playing song state */
	signed short * song = load_song(song_name);
	signed int pins = 0x00000000;
	signed int sample = 0;
	for(int i = 0; i < song_size/2; i++){
		sample = song[i];
		if(song[i] < 0){																/* Proceess sign using the inverter */
			sample *= -1;
			gpio_write(inv_pins[0], 1);
			gpio_write(inv_pins[1], 1);
		}
		else{
			gpio_write(inv_pins[2], 1);
			gpio_write(inv_pins[3], 1);
		}
		pins = (uint8_t)(sample >> 8);
		timer_delay_us(7);																/* Delay to match average sampling frequency */
		for (int i = 0; i< 8; i++){      
            if (pins & 1){
                gpio_write(dac_pins[7-i], 1);											/* Proccess value to analof voltage using DAC */
            }
            pins = pins >> 1;
		}
        for (int i = 0; i< 8; i++){
             gpio_write(dac_pins[i], 0);
        }
        if (song[i] < 0){																/* resets alternator */
            gpio_write(inv_pins[0], 0);
			gpio_write(inv_pins[1], 0);
        }
        else{
			gpio_write(inv_pins[2], 0);
    	    gpio_write(inv_pins[3], 0);
		}

		if(!gpio_read(button_pins[2])){    				 //pause
			printf("paused");
			timer_delay(1);
			display_current(song_name, 0, 0);
			while(gpio_read(button_pins[2])){			 //until play
				if(!gpio_read(button_pins[1])){
					timer_delay_ms(500);
					return 0;							//return to song selection
				}
				else if(!gpio_read(button_pins[0])){
					timer_delay_ms(500);
					return 1;							 //play next song
				}
				else if(!gpio_read(button_pins[3])){
					i = 0;
				}
			}
			display_current(song_name, 0, 1);
			printf("play");
		}
		else if(!gpio_read(button_pins[1])) return 0;	//return to song selection
		else if(!gpio_read(button_pins[0])) return 1;	//play next song
		else if(!gpio_read(button_pins[3])){
			i = 0;
		}
	}
	return 1;
}

