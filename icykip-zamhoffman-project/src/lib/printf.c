#include "printf.h"
#include "printf_internal.h"
#include "strings.h"
#include <stdarg.h>
#include "uart.h"

#define MAX_OUTPUT_LEN 1024
//Checks how much space is required for a given int and its base
int reqbuf_int(int num, int base) { 
	int count = 0;
	if(num == 0){return 1;}
	//if(base == 16){ count += 2;}
	while(num !=  0) {
		count++;
		num /= base;
	}	
	return count;	
}

int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, int min_width)
{
    int size = reqbuf_int(val, base);
    if(min_width > size){size = min_width;}
    char place[size]; 
    char * store = place;
    if(base == 16) { //if the base is hex add the hex indicator 0x
	//*store = '0';
	//store += 1;
	//*store = 'x';
	//store += 1;
	//size -= 2;
    }
    char *end = store + size; //start from the least significant bit
    *end = '\0'; //place end pointer
    end -= 1;
    int count = 1;
    int digit = 0;
    while(end != store - 1){
	digit = val % base; 
	if (digit > 9){ //if hexidecimal and number > 10 conver to character using ascii
		*end = (char)(((digit) % 10) + '0') + 49;
	}
	else{ //otherwise devise the related char
		*end = (char)(digit +  '0');
	}
	if(val != 0){ // shift val to retrive the next digit to transform to char
		val /= base;
		count++;
	} 
	end -= 1;
    }  
    while(val != 0){ // make sure you get the total amount of space for proposed transform
	count++;
	val /= base;
    }
    memcpy(buf, place, bufsize - 1);
    *(buf + bufsize - 1) = '\0';  
    return count;
}

int signed_to_base(char *buf, size_t bufsize, int val, int base, int min_width)
{	
    int result = 0;
    if(val < 0){ //add a minus sign to the front if val is negative                                                                                                 
    	*buf = '-';
    	buf += sizeof(char);
        val *= -1;
	bufsize -= 1;
	result += 1;
	min_width -= 1;
    }
    result += unsigned_to_base(buf, bufsize, val, base, min_width);
    return (result);
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    char filler[bufsize]; //extra memory to de-rail buf iterations but still count intended number of characters written
    int count = 0; //Counts the number of characters written
    char * buf_iter = buf; //allows us to iter through the characters in buf without loosing the location of the beginning of the string
    const char * format_iter = format; //allows us to iter through format without loosing the location of the beginning of  the string
    const char  * format_finder = NULL; //extra function to help find the next location in format after passing it to a function
    int temp = 0; //used to store a variable argument I want to use more than once
    int base = 0; //the base for a number formating dealing with hex or decimal ints
    int width = 0; //used to set the width for hex or decimal functions
    while(*format_iter != '\0'){
	if(*format_iter == '%') {
		format_iter += 1;
		if(*format_iter == 'c'){ //if format val is char simply place char in that byte of buf
			*buf_iter = (char)va_arg(args, int);
			buf_iter += 1;
			count++;
		}	
		else if(*format_iter == '%'){ //if format val is percent sign place it in the respective byte of buf
			*buf_iter = '%';
			buf_iter += 1;
			count++;
		}
		else if(*format_iter == 's') { //if string concatinate the current buf string with the format val
			*buf_iter = '\0';
			count += strlcat(buf, va_arg(args, char *), bufsize - strlen(buf));
			buf_iter += strlen(buf_iter);
		} 
		else if (*format_iter == 'd' || *format_iter == 'x' || (*format_iter >= '0' && *format_iter <= '9' )){	//if the format declaration points toward an int of some kind

			if(*format_iter >= '0' && * format_iter <= '9'){ //if there is a width parameter set width to it
					width = strtonum(format_iter, &format_finder);
					format_iter = format_finder;
			}

			if (*format_iter == 'd'){ //set base to ten if it i a decimal
				base = 10; 
			}
			else { //else set base to hex
				 base = 16;
			 }
	
			*buf_iter = '\0'; 
			temp = va_arg(args, int);

			if (width == 0){ width = reqbuf_int(temp, base);}	//if there is no width param, set width to the required space for the int

			count += signed_to_base(buf_iter, width+1, temp, base, width); 
			buf_iter += strlen(buf_iter);  
			width = 0;  //reset width to use later
		}
		else if(*format_iter == 'p'){ //if pointer, return the hex int as a string
			temp = va_arg(args, int); 
			*buf_iter = '0';
			buf_iter += 1;
			*buf_iter = 'x';
			buf_iter += 1;
			count += unsigned_to_base(buf_iter, MAX_OUTPUT_LEN - strlen(buf), temp, 16, 8);
			buf_iter += reqbuf_int(temp, 16);
		}
		else {
			*buf_iter = *format_iter;
			buf_iter += sizeof(char);
			count++;	
		}
		format_iter += 1;		
	}
	else { //if no fromat request is made simply place what is in format in buf
		*buf_iter = *format_iter;
		buf_iter += sizeof(char);
		format_iter += sizeof(char);
		count++;	
	}	
	if(buf_iter == buf + bufsize-1){ buf_iter = filler; } //if buf is full(reached mem limit) continue until fomrat is finished but instead updating an empty memory storage space.
    }
    *buf_iter = '\0';
    va_end(args);
    return count;
}

int snprintf(char *buf, size_t bufsize, const char *format, ...)
{	
	va_list args;
	va_start(args, format);
	return vsnprintf(buf, bufsize, format, args);
}

int printf(const char *format, ...)
{
    char buf[1024];
    int bufsize = sizeof(buf);
    va_list args;
    va_start(args, format);
    int out = vsnprintf(buf, bufsize, format, args);
    uart_putstring(buf);
    return out;
}
