#include <stdbool.h>
#include "gpio_interrupts.h"
#include "assert.h"
#include "printf.h"
#include "gpioextra.h"

volatile unsigned int *EDS_1 = (unsigned int*)0x20200040;
volatile unsigned int *EDS_2 = (unsigned int*)0x20200044;


/*
 * Module to configure GPIO interrupts for Raspberry Pi.
 * Because all of the GPIO pins share a small set of GPIO
 * interrupts, you need a level of indirectiom to be able
 * to handle interrupts for particular pins. This module
 * allows a client to register one handler for each
 * GPIO pin.
 *
 * Author: Philip Levis <pal@cs.stanford.edu>
 *
 * Last update:   May 2020
 */
static struct{
    handler_fn_t fn;
}handlers[GPIO_PIN_LAST+1];


bool gpio_default_handler(unsigned int pc) {
    int pin = 0;
    while(pin < 54) {
	if(gpio_check_and_clear_event(pin)){
	    return handlers[pin].fn(pc);
	}
	pin++;
    }
    return 0;
}

/*
 * `gpio_interrupts_init`
 *
 * Initialize the GPIO interrupt modules. The init function must be 
 * called before any calls to other functions in this module.
 * The init function configures gpio interrupts to a clean state.
 * This function registers a handler for GPIO interrupts, which it
 * uses to call handlers on a per-pin basis.
 *
 * Disables GPIO interrupts if they were previously enabled.
 * 
 */
void gpio_interrupts_init(void) 
{
    interrupts_init();
    gpio_interrupts_disable(); //disable all gpio interrupts
    interrupts_register_handler(INTERRUPTS_GPIO3, gpio_default_handler); //will call handlers on a per pin basis
}

/*
 * `gpio_interrupts_enable`
 *
 * Enables GPIO interrupts.
 */
void gpio_interrupts_enable(void) 
{
    interrupts_enable_source(INTERRUPTS_GPIO3);
}

/*
 * `gpio_interrupts_disable`
 *
 * Disables GPIO interrupts.
 */
void gpio_interrupts_disable(void) 
{
    interrupts_disable_source(INTERRUPTS_GPIO3); 
}

/* 
 * `interrupts_attach_handler`
 *
 * Attach a handler function to a given GPIO pin. Each pin
 * source can have one handler: further dispatch should be invoked by
 * the handler itself. Whether or not a particular pin will
 * generate interrupts is specified by the events system,
 * defined in `gpioextra.h`. 
 *
 * Asserts if failed to install handler (e.g., the pin is invalid).
 * Pins are defined in `gpio.h`.
 */
handler_fn_t gpio_interrupts_register_handler(unsigned int pin, handler_fn_t fn) 
{
    assert(pin >= GPIO_PIN_FIRST && pin <= GPIO_PIN_LAST);
    gpio_set_input(pin);  
    gpio_enable_event_detection(pin, GPIO_DETECT_FALLING_EDGE);  
    handler_fn_t old_handler = handlers[pin].fn;
    handlers[pin].fn = fn;
    return old_handler;
}

