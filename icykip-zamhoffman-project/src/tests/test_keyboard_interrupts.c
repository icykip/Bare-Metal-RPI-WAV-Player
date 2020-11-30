#include "interrupts.h"
#include "gpio.h"
#include "keyboard.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "gpio_interrupts.h"

static unsigned int breaker = 0;

void test_clock_events(void)
{
    printf("Type on your PS/2 keyboard to generate clock events. You've got 10 seconds, go!\n");
    timer_delay(10);
    printf("Time's up!\n");
}

bool interrupted(unsigned int pc){
    printf("INTERRUPT WORKED\n");
    return 1;
}

bool break_now(unsigned int pc){
    breaker = 1;
    return 1;
}

void test_gpio_int(void){
    gpio_interrupts_init();
    gpio_interrupts_register_handler(GPIO_PIN17, interrupted);
    gpio_interrupts_register_handler(GPIO_PIN27, break_now);
    gpio_interrupts_enable();
    interrupts_global_enable();
    printf("all peices enabled\n");
    while(breaker == 0){
	timer_delay(3);
	printf("Stuck in loop\n");
	continue;
    }
    printf("out of loop\n");
    gpio_interrupts_disable();
    timer_delay(10);
    gpio_interrupts_enable();
    timer_delay(10);
}
/*
 * This function tests the behavior of the assign5 keyboard
 * implementation versus the new-improved assign7 version. If using the
 * assign5 implementation, a key press that arrives while the main program
 * is waiting in delay is simply dropped. Once you upgrade your
 * keyboard implementation to be interrupt-driven, those keys should
 * be queued up and can be read after delay finishes.
 */
void test_read_delay(void)
{
    while (1) {
        printf("Test program waiting for you to press a key (q to quit): ");
        uart_flush();
        char ch = keyboard_read_next();
        printf("\nRead: %c\n", ch);
        if (ch == 'q') break;
        printf("Test program will now pause for 1 second... ");
        uart_flush();
        timer_delay(1);
        printf("done.\n");
    }
    printf("\nGoodbye!\n");
}


void main(void)
{
    gpio_init();
    timer_init();
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

//    test_gpio_int();
    test_clock_events();  // wait 10 seconds for clock_edge handler to report clock edges
    test_read_delay();  // what happens to keys typed while main program blocked in delay?
    uart_putchar(EOT);
}
