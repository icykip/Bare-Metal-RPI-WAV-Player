#include "gpio.h"
#include "timer.h"

void main(void)
{
	unsigned int numbers[16] = {0b00111111, //0
		                    0b00000110, //1
				    0b01011011, //2
                                    0b01001111, //3
                                    0b01100110, //4
          		            0b01101101, //5
 			            0b01111101, //6
			            0b00000111, //7
			            0b01111111, //8
			            0b01100111, //9
			            0b01110111, //A
          		            0b01111100, //B
			            0b00111001, //C
			            0b01011110, //D
	   	   	            0b01111001, //E
			            0b01110001 }; //F
 

	void set_number_on(unsigned int num){
		for (unsigned int i = 20; i < 28; i++){
			if (gpio_get_function(i) != GPIO_FUNC_OUTPUT) {
				gpio_set_output(i); //sets pins to outputs if they have not already been set.
			}
			if (num & (1 << (i % 20))){
        			gpio_write(i, 1); //turns on each gpio pin to generate a given number.
			} 
		}
	}	

	void set_number_off(unsigned int num){
      		for (unsigned int i = 20; i < 28; i++) {
			if (num & (1 << (i % 20))){
        			gpio_write(i, 0); //turns off gpio pins fot previously set number
			}	 	
      		}
	}

	void turn_on_digit(unsigned int digit){	
		if (gpio_get_function(digit) != GPIO_FUNC_OUTPUT) {
			gpio_set_output(digit+10); //sets gpio pins for digit to outputs if they have not already been
		}
		gpio_write(13 - digit, 1);  //turns on a digit so that the set number is displayed
	}
  	  
	void turn_off_digit(unsigned int digit){	
		gpio_write(13 - digit, 0); //turns off a given digit that was previously on.
	}  	  
      
	//returns the number you want to display given a digit and the number of seconds past.

	unsigned int min_sec_selector(unsigned int digit, unsigned int seconds){
		if (digit == 0){
			return (seconds % 10); //returns the digit for the second ones place.
		}
		else if (digit == 1) {
			return (seconds/10 % 6); //returns the digit for the seconds tens place.
		} 
		else if (digit == 2) {
			return (seconds/60 % 10); //returns the number for the minutes ones place.
		}
		else {
			return (seconds/600 % 6); //returns the number for the minutes tens place.
		}
      	}
	
	//turns on and off a digit and number

      	void flash_light(unsigned int digit, unsigned int display){
		turn_on_digit(digit);
		set_number_on(numbers[display]);
		timer_delay_us(2500);
		set_number_off(numbers[display]);
		turn_off_digit(digit);
					
      	}	

	// this funciton allows you to set the time.

	unsigned int set_time(){
		timer_delay_ms(300); // wait after the long button press as to not accidentily begin changing values.
		unsigned int set_time = 0; 
		unsigned int digit = 0;
		unsigned int display = 0;
		
		for(;;){
			if(!gpio_read(GPIO_PIN2) && !gpio_read(GPIO_PIN3)){
				return(set_time); //if both buttosn are pressed return the time you set
			}
			else if(!gpio_read(GPIO_PIN3)){
				set_time += 1;	//if the right most button is pressed increase seconds
				timer_delay_ms(200); //delay time to not pickup residual button presses
			}
			else if(!gpio_read(GPIO_PIN2)){
				set_time += 60; //if the left most button is pressed increase minutes
				timer_delay_ms(200); //de;ay time to not pickup residual button presses
			}
			display = min_sec_selector(digit, set_time); //get the number for the current digit
			flash_light(digit, display);	//flash the current digit and number combination
			if (digit == 3) {
				digit = 0;
			}
			else {
				digit += 1;
			}
		}	
	}
	
	for(;;){
		unsigned int digit = 0;
		unsigned int dash = 0b01000000;

		//at the beginning all digits start as dashs, clock begins on button press

		while(gpio_read(GPIO_PIN2) && gpio_read(GPIO_PIN3)){
			turn_on_digit(digit);
			set_number_on(dash);
			timer_delay_us(5);
			turn_off_digit(digit);
			if (digit == 3) {
				digit = 0;
			}
			else {
				digit += 1;
			}		
		}	

		// clock begins		

		set_number_off(dash);
		unsigned int start = timer_get_ticks();
		unsigned int set_t = 0;
		digit = 0;
		unsigned int display = 0;
		unsigned int seconds = 0;
		unsigned int long_press = 0;
		for(;;){

			//if the rigthmost button is long pressed, you will enter the set time function where you can set the time
				
			if(!gpio_read(GPIO_PIN3)){
				while(!gpio_read(GPIO_PIN3)){
					turn_on_digit(long_press);
					set_number_on(dash);
					timer_delay_ms(500);
					set_number_off(dash);
					turn_off_digit(long_press);
					if(long_press == 3){
						set_t = set_time();
						start = timer_get_ticks();
						long_press = 0;
					}
					long_press += 1;
				}
				long_press = 0;
			}
			seconds = (timer_get_ticks() - start)/1000000 + set_t; //calculates the time to be displayed based on what the set time is and how much time has passed since
			display = min_sec_selector(digit, seconds);
			flash_light(digit, display);
			if (digit == 3) {
				digit = 0;
			}
			else {
				digit += 1;
			}
			
		}
		
	}
}	
