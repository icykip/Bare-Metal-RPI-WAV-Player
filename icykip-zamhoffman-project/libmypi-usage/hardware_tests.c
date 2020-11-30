#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "strings.h"
#include "malloc.h"
#include "keyboard.h"
#include "pi.h"
#include "gpio_ref.h"
#include "gpio.h"
#include "timer.h"
#include "pmi.h"
#include "ff.h"


#define LINE_LEN 80
#define CMD_LEN 6

static int cmd_dacTest(int argc, const char *argv[]);
static int cmd_playTone(int argc, const char *argv[]);
static int cmd_playSong(int argc, const char *argv[]);
static int cmd_playFileSys(int argc, const char *argv[]);

static formatted_fn_t shell_printf;
static const command_t commands[] = {
    {"help",   "<cmd> prints a list of commands or description of cmd", cmd_help},
    {"reboot", "<NONE> restarts your Pi", cmd_reboot},
    {"DAC_test", "<NONE> tests to make sure our dac system is itterating through voltages correctly", cmd_dacTest},	
    {"play_c", "<NONE> restarts your Pi", cmd_playTone},
    {"play_song", "<NONE> Plays a song", cmd_playSong},
    {"play", "<song> plays a song from the sd card", cmd_playFileSys} 
};
 
static int sample_rate = 44100;
static int freq = 262;
static double pi = 3.1415926535;
 
static double exp(double x, int p)
{
    if(p == 0) return 1;
    double e = x;
    for(int i = 0; i< p-1; i++){
       e *= x;
    }
    return e;
}

static int fact(int f)
{
    int num = 1;
    for(int i = 1; i <= f ; i++){
       num *= i;
    }
    return num;
}

static double sin(double x)
{
    int count = 1;
    double num = x;
    for (int i = 3; i < 10 ; i +=2){
        num += (exp(-1, count) * exp(x, i) / fact(i));
        count ++;
    }
    return num;
}
  
//given some frequency and amps, this will return one period of the sin wave. 
//user should loop over the returned array to play a sound at bit rate of 32 bits every 1/sample_rate seconds
static double * mono_wave(int amp, double buf[])
{
    int steps = sample_rate/freq;
    for (int i = 0; i< steps; i++) {
             buf[i] = amp * sin((double)freq * i * 2 * pi/ (double)sample_rate);
    }
    return buf;
}

static int cmd_playFileSys(int argc, const char *argv[]){
    
    if(argc != 2){
	shell_printf("%s takes one argument - the song you want played", argv[0]);
	return 1;
    }
    FATFS *fs;     /* Ponter to the filesystem object */
    fs = malloc(sizeof (FATFS));           /* Get work area for the volume */
    f_mount(fs, "", 0);    

    FRESULT res;
    FIL * file;
    file = malloc(sizeof (FIL));
    shell_printf("song: %s", argv[1]);
    res = f_open(file, argv[1], FA_READ);
    if (res != FR_OK) {
        shell_printf("Could not create file %s. Error: %s\n", argv[1], ff_err_msg(res));
        return 1;
    }
    unsigned int size = (int)f_size(file);
    signed char * song = malloc((size - 4500) * sizeof(char));
    f_lseek(file, 44);
    unsigned int * nread;
    res = f_read(file, song, size - 4499, nread);
    if (res != FR_OK || *nread != (size-4499)) {
        shell_printf("Could not read from file %s. Error: %s\n",argv[1], ff_err_msg(res));
        return 1;
    }
    f_close(file);
    shell_printf("File output: %d\n", size-4499);
//    for(int x = 0; x < size-44; x += 10){
//	shell_printf("%d, ", song[x]);
  //  }
   // return 0;
    song[size-4499] = song[0];
    int gpio[8] = {GPIO_PIN19, GPIO_PIN13, GPIO_PIN6, GPIO_PIN5, GPIO_PIN1, GPIO_PIN7, GPIO_PIN8, GPIO_PIN25};

    gpio_set_function(GPIO_PIN1, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN7, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN8, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN25, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN19, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN13, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN6, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN5, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN21, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN16, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN12, GPIO_FUNC_OUTPUT);

    for (int i = 0; i< 8; i++){      //turn off all pins
            gpio_write(gpio[i], 0);
    }

    int count = 0;
    int pins = 0x00000000;
 
    while(1){
        if (count == (size-4499)) count = 0;
        double sample = song[count];
        pins = (sample);
        if (sample < 0){                        //handle positive, negative alternator
            pins *= -1;
            gpio_write(GPIO_PIN20, 1);
            gpio_write(GPIO_PIN21, 1);
        }
        else{
            gpio_write(GPIO_PIN16, 1);
            gpio_write(GPIO_PIN12, 1);
        }
        for (int i = 0; i< 8; i++){      //turn on appropriate pins
            if (pins & 1){
                gpio_write(gpio[7-i], 1);
            }
            pins = pins >> 1;
        }
	//timer_delay_us(7);
        for (int i = 0; i< 8; i++){
             gpio_write(gpio[i], 0);
        }
        if (sample < 0){                 //resets alternator
            gpio_write(GPIO_PIN20, 0);
            gpio_write(GPIO_PIN21, 0);
        }
        else{
            gpio_write(GPIO_PIN16, 0);
            gpio_write(GPIO_PIN12, 0);
        }
        count ++;
        pins = 0;
    }


}

static int cmd_playSong(int argc, const char *argv[])
{
    if(argc > 1){
        shell_printf("%s takes no argumetns", argv[0]);
        return 1;
    }
    //int steps = sample_rate/freq;
    //double * wave = malloc(steps * sizeof(double));
    //wave = mono_wave(1, wave);

    int gpio[8] = {GPIO_PIN19, GPIO_PIN13, GPIO_PIN6, GPIO_PIN5, GPIO_PIN1, GPIO_PIN7, GPIO_PIN8, GPIO_PIN25};

    gpio_set_function(GPIO_PIN1, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN7, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN8, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN25, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN19, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN13, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN6, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN5, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN21, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN16, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN12, GPIO_FUNC_OUTPUT);

    for (int i = 0; i< 8; i++){      //turn off all pins
            gpio_write(gpio[i], 0);
    }

    int steps = sizeof(pmi)/sizeof(pmi[0]);
    int count = 0;
    int pins = 0x00000000;
    while(1){
        if (count == steps - 1) count = 0;
        double sample = pmi[count];
        pins = (sample / 256);
        if (sample < 0){                        //handle positive, negative alternator
            pins *= -1;
            gpio_write(GPIO_PIN20, 1);
            gpio_write(GPIO_PIN21, 1);
        }
        else{
            gpio_write(GPIO_PIN16, 1);
            gpio_write(GPIO_PIN12, 1);
        }
        for (int i = 0; i< 8; i++){      //turn on appropriate pins
            if (pins & 1){
                gpio_write(gpio[7-i], 1);
            }
            pins = pins >> 1;
        }
	timer_delay_us(6);
        for (int i = 0; i< 8; i++){
             gpio_write(gpio[i], 0);
        }
        if (sample < 0){                 //resets alternator
            gpio_write(GPIO_PIN20, 0);
            gpio_write(GPIO_PIN21, 0);
        }
        else{
            gpio_write(GPIO_PIN16, 0);
            gpio_write(GPIO_PIN12, 0);
        }
        count ++;
        pins = 0;
    }

}

static int cmd_playTone(int argc, const char *argv[])
{
    if(argc > 1){
        shell_printf("%s takes no argumetns", argv[0]);
        return 1;
    }
    int steps = sample_rate/freq;
    double * wave = malloc(steps * sizeof(double));
    wave = mono_wave(1, wave);

    int gpio[8] = {GPIO_PIN19, GPIO_PIN13, GPIO_PIN6, GPIO_PIN5, GPIO_PIN1, GPIO_PIN7, GPIO_PIN8, GPIO_PIN25};

    gpio_set_function(GPIO_PIN1, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN7, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN8, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN25, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN19, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN13, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN6, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN5, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN21, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN16, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN12, GPIO_FUNC_OUTPUT);

    for (int i = 0; i< 8; i++){      //turn off all pins
            gpio_write(gpio[i], 0);
    }

    int count = 0;
    int pins = 0x00000000;
    while(1){
        if (count == steps - 1) count = 0;
        double sample = wave[count];
        pins = (sample * 256);
        if (sample < 0){                        //handle positive, negative alternator
            pins *= -1;
            gpio_write(GPIO_PIN20, 1);
            gpio_write(GPIO_PIN21, 1);
        }
        else{
            gpio_write(GPIO_PIN16, 1);
            gpio_write(GPIO_PIN12, 1);
        }
        for (int i = 0; i< 8; i++){      //turn on appropriate pins
            if (pins & 1){
                gpio_write(gpio[7-i], 1);
            }
            pins = pins >> 1;
        }
	timer_delay_us(20);
        for (int i = 0; i< 8; i++){
             gpio_write(gpio[i], 0);
        }
        if (sample < 0){                 //resets alternator
            gpio_write(GPIO_PIN20, 0);
            gpio_write(GPIO_PIN21, 0);
        }
        else{
            gpio_write(GPIO_PIN16, 0);
            gpio_write(GPIO_PIN12, 0);
        }
        count ++;
        pins = 0;
    }

}
int cmd_dacTest(int argc, const char *argv[]){
    if(argc > 1){
    	shell_printf("%s takes no argumetns", argv[0]);
        return 1;
    }
    gpio_set_function(GPIO_PIN1, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN7, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN8, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN25, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN19, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN13, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN6, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN5, GPIO_FUNC_OUTPUT);
    
    char c = keyboard_read_next();
    while(c != '\n'){
    	if(c == 'a'){
	    shell_printf("Bit 1 toggle\n");
	    if(gpio_read(GPIO_PIN19)) gpio_write(GPIO_PIN19, 0);
	    else gpio_write(GPIO_PIN19, 1);
    	}
    	else if(c == 's'){
	     shell_printf("Bit 2 toggle\n");
	     if(gpio_read(GPIO_PIN13)) gpio_write(GPIO_PIN13, 0);
	    else gpio_write(GPIO_PIN13, 1);
    	}
	else if(c == 'd'){
	    shell_printf("Bit 3 toggle\n");
	    if(gpio_read(GPIO_PIN6)) gpio_write(GPIO_PIN6, 0);
	    else gpio_write(GPIO_PIN6, 1);
    	}
	else if(c == 'f'){
	    shell_printf("Bit 4 toggle\n");
	    if(gpio_read(GPIO_PIN5)) gpio_write(GPIO_PIN5, 0);
	    else gpio_write(GPIO_PIN5, 1);
    	}
	else if(c == 'z'){
	    shell_printf("Bit 5 toggle\n");
	    if(gpio_read(GPIO_PIN1)) gpio_write(GPIO_PIN1, 0);
	    else gpio_write(GPIO_PIN1, 1);
    	}
	else if(c == 'x'){
	    shell_printf("Bit 6 toggle\n");
	    if(gpio_read(GPIO_PIN7)) gpio_write(GPIO_PIN7, 0);
	    else gpio_write(GPIO_PIN7, 1);
    	}
	else if(c == 'c'){
	    shell_printf("Bit 7 toggle\n");
	    if(gpio_read(GPIO_PIN8)) gpio_write(GPIO_PIN8, 0);
	    else gpio_write(GPIO_PIN8, 1);
    	}
	else if(c == 'v'){
	    shell_printf("Bit 8 toggle\n");
	    if(gpio_read(GPIO_PIN25)) gpio_write(GPIO_PIN25, 0);
	    else gpio_write(GPIO_PIN25, 1);
    	}
        c = keyboard_read_next();	
    }
    return 0;
}

static const char * test_cases[4][6] = {
{"help", "help echo", "help 	peek", "help NOT", "hell", "  "},
{"echo", "echo Hello, World!", "  echo    no more     spaces  dont count wrong", " ", "echoooo", "e c h o"},
{"peek", "   peek 0xFFF8", "peek 0x0",  "peek bob", "  ", "peek 0xFFFC"},
{"poke", "poke 0xFFF8 1", "peek 0xFFF8", "poke 0xFFF8 0", "peek 0xFFF8", "poke bob 3"}
};

int cmd_test(int argc, const char *argv[]){
	if(argc > 1){
		shell_printf("%s takes no argumetns", argv[0]); 
		return 1;
	}
	for(int i = 0; i < CMD_LEN-2; i++){
		shell_printf("----- test %s -----\n", commands[i].name);
		for(int x = 0; x < 6; x++){
			shell_printf("input: %s\n", test_cases[i][x]);
			shell_evaluate(test_cases[i][x]);
		}
	}
	shell_printf("tests complete\n");
	return 0;
}

int cmd_echo(int argc, const char *argv[]) 
{
    for (int i = 1; i < argc; ++i) 
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[]) 
{
    if(argc < 2){
    	for(int i = 0; i < CMD_LEN; i++){
		shell_printf("%s: %s\n", commands[i].name, commands[i].description);
    	}
	return 0;
    }
    else{
	for(int i = 0; i < CMD_LEN; i++){
		if(strcmp(commands[i].name, argv[1]) == 0){
			shell_printf("%s: %s\n", commands[i].name, commands[i].description);
			return 0; 
		}
    	}
	shell_printf("\nerror: no such command '%s'.\n", argv[0]);
	return -1;
    }
    return -1;
}

int cmd_reboot(int argc, const char *argv[]){
    if(argc > 1) shell_printf("%s does not take any additional arguments", argv[0]);
    uart_putchar(EOT);
    pi_reboot();
    return 0;
}	

int cmd_peek(int argc, const char *argv[]){
     if(argc != 2){
	shell_printf("error: peek expects 1 argument [address]\n");
	return 1;
     }
     else{
	int * addr = (int*)strtonum(argv[1], NULL);
	if((int)addr == 0 && strcmp(argv[1], "0x0") != 0){
	    shell_printf("error: peek cannot convert '%s'\n", argv[1]);
	    return 1;
	}
	else if((int)addr % 8 != 0){
	    shell_printf("error: peek address must be 4-byte aligned\n");
	    return 1;
        }
	else{
	    shell_printf("%p: %d\n", addr, *addr);
	    return 0;
	}
    }
}

int cmd_poke(int argc, const char *argv[]){
    if(argc != 3){
	shell_printf("poke requires 2 arguments [address][value]\n");
	return 1;
    }
    else{
	int * addr = (int*)strtonum(argv[1], NULL);
	int val = strtonum(argv[2], NULL);
	if((int)addr == 0 && strcmp(argv[1], "0x0") != 0){
	    shell_printf("error: peek cannot convert '%s'\n", argv[1]);
	    return 1;
	}
	else if((int)addr % 8 != 0){
	    shell_printf("error: peek address must be 4-byte aligned\n");
	    return 1;
        }
	else{
	    *addr = val;
	    return 0;
	}
    }
}


void shell_init(formatted_fn_t print_fn)
{
    shell_printf = print_fn;
}

void shell_bell(void)
{
    uart_putchar('\a');
}

void shell_readline(char buf[], size_t bufsize)
{
    // TODO: your code here
    int len = 0;
    char c = keyboard_read_next(); 
    while(c != '\n'){
	if(c == '\b'){
		if(len == 0) shell_bell();
		else{
			shell_printf("%c", c);
			shell_printf(" ");
			shell_printf("%c", c);
			len--;
			buf[len] = '\0'; 	
		}
	}
	else if(len == bufsize-1) shell_bell();
	else{
		buf[len] = c;
		shell_printf("%c", c);
		len++;
	}
	c = keyboard_read_next();
    }
    buf[len] = '\0';
}

int shell_evaluate(const char *line)
{
    //tokenize	
    const char * start = line;
    const char * end = line;
    unsigned int spot = 0;
    const char * tokens[strlen(line)];
    while(*end == ' '){
	if(end == '\0'){
		shell_printf("\nerror: no such command '%s'.\n", tokens[0]);
		return -1;
	}
	end += 1;
	start += 1;	
    }
    while(1){
	char * token;
	if(*end == ' ' || *end == '\t' || *end == '\0'){
		token = malloc(end-start);
		memcpy(token, start, end-start);
		*(token + (end-start)) = '\0';
		tokens[spot] = token;
		spot++;
		if(*end == '\0') break;
		while(*end == ' ' || *end == '\t') end += 1;
		start = end;		
	}
	end += 1;
    }
    shell_printf("\n");
		    
    //evaluate
    for(int i = 0; i < CMD_LEN; i++){
	if(strcmp(commands[i].name,tokens[0]) == 0) return commands[i].fn(spot, tokens);
    }
    shell_printf("\nerror: no such command '%s'.\n", tokens[0]);
    return -1; 
}

void shell_run(void)
{
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1) 
    {
        char line[LINE_LEN];

        shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}
