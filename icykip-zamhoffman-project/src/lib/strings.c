#include "strings.h"

void *memset(void *s, int c, size_t n)
{	
    unsigned char * s_copy = s;
    
    for(int i = 0; i  < n; i++) {
	*s_copy = (unsigned char)c; //place value in mem
	s_copy = s_copy + sizeof(unsigned char); //increment mem location until desired space is written to 
    }
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    const unsigned char * src_copy = src;
    unsigned char * dst_copy = dst;
    for (int i = 0; i < n; i ++){
	*dst_copy = *src_copy; //set the byte in the destination to the byte in the source
	dst_copy += sizeof(unsigned char); //increment both the detination and source pointer
	src_copy += sizeof(unsigned char);
    } 
    return dst;
}

size_t strlen(const char *s)
{
    /* Implementation a gift to you from lab3 */
    int n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

int strcmp(const char *s1, const char *s2)
{
    int n = 0;
    while (s1[n] == s2[n] && s1[n] && s2[n]) {
	n++; //increments until there is a difference in character or one ends
    }
    return (s1[n] - s2[n]);  //then proceedes to compare those twocharacters
}

size_t strlcat(char *dst, const char *src, size_t maxsize)
{	
    unsigned int out = strlen((char*)dst) + strlen((char*)src);
    char * ptr = dst + strlen(dst);
    size_t end = 0;
    //sets to the end  of the string to whatever is shorter(maxsize or the string size)
    if(maxsize-strlen(dst) > strlen(src)){
	end = strlen(src);
    }
    else{ end = maxsize - strlen(dst); }
    //copys the src string to the end of the dst string
    ptr = memcpy(ptr, src, end);
    *(ptr + end) = '\0';
    return out;
}

int isdigit(char c){ //checks if c is a decimal digit
	if(c - '0' >= 0 && c - '0' < 10){ return 1; }
	return 0;
}

int isxdigit(char c){ //check if c is a hex digit
	if ((c - '0' >= 0 && c - '0' < 10) || (c >= 'a' && c <= 'f')){
		return 1;
	}
	return 0; 
}

int pow(int exp, int base) { //does an exponentioal expression and returns the result
	unsigned int ans = 1;
	while(exp > 0){
		ans *= base;
		exp -= 1;
	}
	return ans;
}

unsigned int strtonum(const char *str, const char **endptr)
{
    int (*func)(char);
    int base;
    if(endptr != NULL){ *endptr = str; } //set endptr to the front if it is not Null
    if(*str == '0' && *(str+1) == 'x'){ //if the string is a hex number  prepare for iut accordingly
	func  = &isxdigit;
	base = 16;
	str += 2; //increment str so you begin at the begging of the actual number
    }
    else if(isdigit(*str)) { //if the string is a decimal prepare fot it accordingly
	func = &isdigit;
	base = 10;
    }
    else{ return 0; }  //if the string is not a decimal or hexidecimal return  
    const char *str_end = str;
    unsigned int length = 0;
    //find the number of digits in the string
    while(*str_end != '\0' && (*func)(*str_end)) {
	str_end += sizeof(unsigned char);
	length++;
    }
    if(endptr != NULL){ *endptr = str_end; } //set endptr to the end of the number if endptr is not NULL
    const char * str_copy = str;
    unsigned int result = 0;
    int hexhandler = 0;
    while (length > 0) { 
	hexhandler =  *str_copy-'0'; //set hexhandler to the int representation of the character
	if((*str_copy - '0') >  9){hexhandler -= 39;} //if the character is a letter(a-f) subtract 39 to get the correct decimal int
	result += hexhandler * pow(length-1, base); //multiply the int by its respective places value given the base and location in the string
	str_copy += sizeof(unsigned char);
	length -= 1;
    }
    return result;	
}
