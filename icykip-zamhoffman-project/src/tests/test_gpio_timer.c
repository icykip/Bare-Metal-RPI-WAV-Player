#include "assert_internal.h"
#include "gpio.h"
#include "timer.h"

void test_gpio_set_get_function(void) {
    gpio_init();

    // Test get pin function (pin2 defaults to input)
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT );   	
    assert( gpio_get_function(GPIO_PIN21) == GPIO_FUNC_INPUT );
    	
    // Test set pin to output
    gpio_set_output(GPIO_PIN2);
    gpio_set_output(GPIO_PIN21);
     gpio_set_output(-1);
    gpio_set_output(GPIO_PIN22);
    
    // Test get pin function after setting
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_OUTPUT );
    assert( gpio_get_function(GPIO_PIN21) == GPIO_FUNC_OUTPUT );
    assert( gpio_get_function(GPIO_PIN22) == GPIO_FUNC_OUTPUT );
    assert( gpio_get_function(-1) == GPIO_INVALID_REQUEST );
    	
    //Test alternate function and changing pin function 
    gpio_set_function(GPIO_PIN2, GPIO_FUNC_ALT0);
    gpio_set_function(GPIO_PIN3, GPIO_FUNC_ALT5);
    gpio_set_function(GPIO_PIN21, 100);
    
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_ALT0 );
    assert( gpio_get_function(GPIO_PIN3) == GPIO_FUNC_ALT5 );
    assert( gpio_get_function(GPIO_PIN21) == GPIO_FUNC_OUTPUT );

    	
}

void test_gpio_read_write(void) {
    gpio_init();
    gpio_set_function(GPIO_PIN3, GPIO_FUNC_OUTPUT);
    //gpio_set_function(GPIO_PIN2, GPIO_FUNC_OUTPUT);
   // gpio_set_function(GPIO_PIN21, GPIO_FUNC_OUTPUT);

    // Test gpio_write low, then gpio_read
    gpio_write(GPIO_PIN3, 0);
    //gpio_write(GPIO_PIN2, 0);
   // gpio_write(GPIO_PIN23, 0);
    assert( gpio_read(GPIO_PIN3) == 0 );
    //assert( gpio_read(GPIO_PIN2) == 0 );	
   // assert( gpio_read(GPIO_PIN21) == 0);    

   // Test gpio_write high, then gpio_read
    gpio_write(GPIO_PIN3, 1);
    //gpio_write(GPIO_PIN2, 1);
    //gpio_write(GPIO_PIN21, 1);
    assert( gpio_read(GPIO_PIN3) ==  1 );
    //assert( gpio_read(GPIO_PIN2) == 1 );
    //assert( gpio_read(GPIO_PIN21) == 1);

   // Test gpio write low again
    gpio_write(GPIO_PIN3, 0);
   // gpio_write(GPIO_PIN2, 0);
   // gpio_write(GPIO_PIN23, 0);
    assert( gpio_read(GPIO_PIN3) ==  0 );
   // assert( gpio_read(GPIO_PIN2) == 0);	
   // assert( gpio_read(GPIO_PIN21) == 0);    
   
   // Test gpio test invalid requests
   // gpio_write(GPIO_PIN20, 3);
   // assert( gpio_read(GPIO_PIN20) == 0 );
   // assert( gpio_read(-1) == GPIO_INVALID_REQUEST );

}

void test_timer(void) {
    timer_init();

    // Test timer tick count incrementing
    unsigned int start = timer_get_ticks();
    for( int i=0; i<10; i++ ) { /* Spin */ }
    unsigned int finish = timer_get_ticks();
    assert( finish > start );

    // Test timer delay
    int usecs = 100;
    start = timer_get_ticks();
    timer_delay_us(usecs);
    finish = timer_get_ticks();
    assert( finish >= start + usecs );
}

// Uncomment each call below when you have implemented the functions
// and are ready to test them

void main(void) {
    test_gpio_set_get_function();
    test_gpio_read_write();
   // test_timer();
}