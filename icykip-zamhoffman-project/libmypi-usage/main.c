#include "printf.h"
#include "uart.h"
#include "keyboard.h"
#include "shell.h"
#include "player.h"
#include "menu.h"
#include "interface.h"
void main(void) 
{
    uart_init();
	gpio_init();
	interface_init(40, 40);
    printf("Hello, world!\n");
	run_music_player();
	//signed char * song = load_song("Mario_theme.wav");
	//play(song);
	printf("finished");
	uart_putchar(EOT);
}
