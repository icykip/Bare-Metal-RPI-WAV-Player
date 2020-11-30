#include "gpio.h"
 
void gpio_init(void) {
}

void gpio_set_function(unsigned int pin, unsigned int function) {
    volatile unsigned int* FSEL = (unsigned int*)(0x20200000 + (pin / 10) * 4); // initialize the correct fselect bsased on the pin 
    if (pin >= GPIO_PIN_FIRST && pin <= GPIO_PIN_LAST && function <= 7 && function >= 0) { // check if the request is valid
	unsigned int temp = 0xFFFFFFFF ^ (0b111 << ((pin % 10) * 3));
	*FSEL = temp & *FSEL; // clears the function select for the given pin
	*FSEL += (function << ((pin % 10) * 3)); //reassigns the function select for the given pin
    }
}


unsigned int gpio_get_function(unsigned int pin) {
    volatile unsigned int* FSEL = (unsigned int*)(0x20200000 + (pin / 10) * 4);
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) {
	return (GPIO_INVALID_REQUEST); // check for an invalid request
    }
    else {
	return (((0b111 << ((pin % 10) * 3)) & *FSEL) >> ((pin % 10) * 3)); //Ands the pin and 0b111 to return the function of a given pin
    }
}

void gpio_set_input(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}
void gpio_set_output(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

void gpio_write(unsigned int pin, unsigned int value) {
    volatile unsigned int* SET = (unsigned int*)(0x02020001C + (pin/32)*4); //point to address required for the pin
    volatile unsigned int* CLR = (unsigned int*)(0x020200028 + (pin/32)*4); //point to address required for the pin 
    if (value == 1){
	*SET = (1 << (pin % 32)); //set pin x to high
    }
    else if(value == 0){
	*CLR = (1 << (pin % 32)); //set pin x to low
    }
}

unsigned int gpio_read(unsigned int pin) { 
    volatile unsigned int* LEV = (unsigned int*)(0x20200034 + (pin/32)*4); // point to address required for pin
    if (pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST){
	return (GPIO_INVALID_REQUEST); // check for invalid requsts
    }
    else {
	if((*LEV & (1 << (pin % 32))) > 0){
		return 1; // if lev holds a value at pin x return 1 else return 0
	}
	else{
		return 0;
	}
    }
}
