#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "strings.h"
#include "malloc.h"
#include "keyboard.h"
#include "pi.h"
#include "gpio_ref.h"
#include "gpio.h"


#define LINE_LEN 80
#define CMD_LEN 7

int cmd_test(int argc, const char *argv[]);
int cmd_dacTest(int argc, const char *argv[]);


static const int commands_space = 30;
static formatted_fn_t shell_printf;
command_t commands[commands_space];
commands[0]  =  {"help",   "<cmd> prints a list of commands or description of cmd", cmd_help};
commands[1] = {"echo",   "<...> echos the user input to the screen", cmd_echo};
commands[2] = {"peek", "<address> prints the 4-byte value stored at memory address", cmd_peek};
commands[3] = {"poke", "<address><value> stores `value` into the memory at `address`.", cmd_poke};
commands[4] = {"test", "runs a series of tests to evaluate the afformaentioned functions", cmd_test}; 
commands[5] = {"reboot", "<NONE> restarts your Pi", cmd_reboot};
commmands[6] = {"DAC_test", "<NONE> tests to make sure our dac system is itterating through voltages correctly", cmd_dacTest};	

static char ** queue = {};
static char * current = {};
static char ** songs ={}

//load all the songs into songs list and display
int cmd_player(int argc, const char *argv[]){
	
}

//delete from queue, Add to queue, display all songs in queue
int cmd_queue(int argc, const char *argv[]){
	 if(argc > 1){
    	shell_printf("%s takes no argumetns", argv[0]);
        return 1;
    }
}

//play song, play song that was paused, play song and create queue, shuffle all songs
int cmd_play(int argc, const char *argv[]){
	 if(argc < 2){
    	shell_printf("%s takes no argumetns", argv[0]);
        return 1;
    }	
	
}

//pause the music
int cmd_pause
// play the next song, if nothing display song options
int cmd_next
// play next song in queue if nothing exit
int cmd_prev
// stop playing song and set queue to 0
int cmd_clear
// creates a playlist, add songs to a playlist and delete playlists
int playlist
// text someone the name of the song I am listenting to (stretch)
int share

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
