#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "timer.h"
#include "printf.h"
#include "gpio_interrupts.h"
#include "assert.h"
#include "malloc.h"

#define RELEASE 0xF0
#define LONG_PRESS 0xE0
#define FULL_MOD 0x1F
 
static unsigned int CLK, DATA;
static unsigned int CURRENT_MODS = 0;

enum { START_BIT = 0, STOP_BIT = 1 };
enum { PS2_CMD_RESET = 0xFF, PS2_CODE_ACK = 0xFA, PS2_CMD_FLAGS = 0xED , PS2_CMD_ENABLE_DATA_REPORTING = 0xF4 };
 
static void falling_clock_edge(void)
{   
     while (gpio_read(CLK) == 0) {}
     while (gpio_read(CLK) == 1) {}
}
 
static int read_bit_init(void)
{
    falling_clock_edge();
    return gpio_read(DATA);
}

static int read_bit(void){return gpio_read(DATA);}

bool has_odd_parity(unsigned char code) {
    unsigned count = 0;
    while(code){
        if(code & 1){
                count++;
        }
        code = code >> 1;
    }
    return ((count + read_bit_init()) % 2) == 1;
}
 
static void write_bit(int nbit, unsigned char code)
{
    switch (nbit) {
         case 1:
            gpio_write(DATA, START_BIT);
            break;
        case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
            gpio_write(DATA, (code >> (nbit-2)) & 1);
            break;
        case 10:
            gpio_write(DATA, has_odd_parity(code));
            break;
        case 11:
            gpio_write(DATA, STOP_BIT);
            break;
    }
}
 
static void ps2_write(unsigned char command)
{   
    gpio_set_output(CLK);   // swap GPIO from read to write
    gpio_set_output(DATA);  
    gpio_write(CLK, 0);     // bring clock line low to inhibit communication
    timer_delay_us(200);    // hold time > 100 us
    gpio_write(DATA, 0);    // initiate request-to-send, this is START bit
    gpio_write(CLK, 1);
    gpio_set_input(CLK);    // release clock line, device will now pulse clock for host to write
      
    for (int i = 2; i <= 11; i++) {  // START bit sent above, now send bits 2-11
        falling_clock_edge();
        write_bit(i, command);
    }
    
    gpio_set_input(DATA);   // done writing, exit from request-to-send
    falling_clock_edge();
}

unsigned int init_scancode(void){
    unsigned int ans = 0;
    unsigned int count = 0;
    while(read_bit_init() !=  0){continue;}
    while(count < 8){
        ans = ans | read_bit_init() << count;
        count++;
    }
    if(!has_odd_parity(ans)) return init_scancode();
    else if(read_bit_init() == 0) return init_scancode();
    return ans;
}

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio) 
{
    CLK = clock_gpio;
    gpio_set_input(CLK);
    gpio_set_pullup(CLK);
 
    DATA = data_gpio;
    gpio_set_input(DATA);
    gpio_set_pullup(DATA); 

    printf("Sending reset code.\n");
    ps2_write(PS2_CMD_RESET);  
    printf("Respond Character: %c\n", init_scancode());

    keyboard_use_interrupts();
}

unsigned int ** ring_buffer;

void set_ring_buffer(void){
    ring_buffer = (unsigned int **)malloc(300 * sizeof(unsigned int *));
    for(int i = 0; i < 300; i++){
    	ring_buffer[i] = (unsigned int*)malloc(11 * sizeof(unsigned int *));
    }
}
 
unsigned int rb_head = 0;
unsigned int rb_tail = 0;
unsigned int sc_part = 0;

bool interrupt_scancode(unsigned int pc){
    ring_buffer[rb_tail][sc_part] = read_bit();
    if(ring_buffer[rb_tail][sc_part] != 0 && sc_part == 0){return 1;}
    if(sc_part == 10){
	assert(rb_tail < 99);
	rb_tail++;
	sc_part = 0;
    }
    else sc_part++;
    return 1;
}


void keyboard_use_interrupts(void){
    gpio_interrupts_init();
    gpio_interrupts_register_handler(CLK, interrupt_scancode);
    gpio_interrupts_enable();
    interrupts_global_enable();
    set_ring_buffer();
}

bool rb_check_odd_parity(unsigned char code, unsigned int parity) {
     unsigned count = 0;
     while(code){
         if(code & 1){
                 count++;
         }
         code = code >> 1;
     }
     return ((count + parity) % 2) == 1;
}

unsigned char keyboard_read_scancode(void) 
{
    unsigned int ans = 0;
    unsigned int count = 0;
    while(rb_head == rb_tail){continue;}
    unsigned int * scan_code = ring_buffer[rb_head];
    while(count < 8){
        ans = ans | scan_code[count + 1] << count;
        count++;
    }
    rb_head++;
    if(rb_head == rb_tail){
	rb_head = 0;
	rb_tail = 0;
    }
    if(!rb_check_odd_parity(ans, scan_code[9]) || !scan_code[10]){
	 return keyboard_read_scancode();
    }
    return ans;
}

key_action_t keyboard_read_sequence(void)
{
    key_action_t action;
    unsigned char first_byte = keyboard_read_scancode();
    if(first_byte == LONG_PRESS){
	first_byte = keyboard_read_scancode();
    }
    if(first_byte == RELEASE){
	action.what = KEY_RELEASE;
	first_byte = keyboard_read_scancode();
	action.keycode = first_byte;
    }
    else
	action.what = KEY_PRESS;
	action.keycode = first_byte;
    return action;
}

void change_modifier(unsigned int mod, unsigned int key_act){
    if(key_act == KEY_PRESS)
	CURRENT_MODS = CURRENT_MODS | mod;
    else
	CURRENT_MODS = CURRENT_MODS & (mod ^ FULL_MOD);
}

void set_modifier(unsigned int mod){
    if(CURRENT_MODS & mod)
	CURRENT_MODS = CURRENT_MODS & (mod ^ FULL_MOD);
    else
	CURRENT_MODS = CURRENT_MODS | mod;
}

key_event_t keyboard_read_event(void) 
{
    // TODO: Your code here
    key_event_t event;
    event.action = keyboard_read_sequence();
    event.key = ps2_keys[event.action.keycode];
    if(event.key.ch == PS2_KEY_SHIFT)
	change_modifier(KEYBOARD_MOD_SHIFT, event.action.what);	    
    else if(event.key.ch == PS2_KEY_ALT)
	change_modifier(KEYBOARD_MOD_ALT, event.action.what);	    
    else if(event.key.ch == PS2_KEY_CTRL)
	change_modifier(KEYBOARD_MOD_CTRL, event.action.what);	    
    else if(event.key.ch == PS2_KEY_CAPS_LOCK && event.action.what == KEY_PRESS)
	set_modifier(KEYBOARD_MOD_CAPS_LOCK);
    else if(event.key.ch == PS2_KEY_SCROLL_LOCK && event.action.what == KEY_PRESS)
	set_modifier(KEYBOARD_MOD_SCROLL_LOCK);
    else if(event.key.ch == PS2_KEY_NUM_LOCK && event.action.what == KEY_PRESS)
	set_modifier(KEYBOARD_MOD_NUM_LOCK);		
    event.modifiers = CURRENT_MODS;
    return event;
}


unsigned char keyboard_read_next(void) 
{
    // TODO: Your code here
    key_event_t read = keyboard_read_event(); 
    while(read.action.what != KEY_PRESS || read.key.ch == PS2_KEY_SHIFT || read.key.ch == PS2_KEY_CAPS_LOCK)read = keyboard_read_event();
    if(read.modifiers & KEYBOARD_MOD_SHIFT)
	return read.key.other_ch;
    else if((read.modifiers & KEYBOARD_MOD_CAPS_LOCK) && (read.key.ch >= 'a' && read.key.ch <= 'z'))
	return read.key.other_ch; 
    else
	return read.key.ch;
}

/*
 * `keyboard_use_interrupts`
 *
 * Change keyboard from default polling behavior to instead configure interrupts 
 * for gpio events. After setting keyboard to use interrupts, client must
 * also globally enable interrupts at system level.
 */

