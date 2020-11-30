#include "assert.h"
#include "printf.h"
#include "printf_internal.h"
#include <stddef.h>
#include "strings.h"
//#include "uart.h"

static void test_memset(void)
{
   int numA = 0xefefefef;
   int numB = 2;

   memset(&numB, 0xef, sizeof(int));
   assert(numA == numB);

   numA = 0xef;
   numB = 2;
   
   memset(&numB, 0xef, sizeof(char));
   assert(numA == numB);

   numA = 0x34343434;
   numB = 2;
   
   memset(&numB, 0x34, sizeof(int));
   assert(numA == numB);
}

static void test_memcpy(void)
{
   int numA = 0x12345678;
   int numB = 2;
   int numC = 4;
   char charA = 'a';
   char charB = 'b';

   memcpy(&numB, &numA, sizeof(int));
   assert(numA == numB);

   memcpy(&numA, &numC, sizeof(int));
   assert(numA = numC);

   memcpy(&charA, &charB, sizeof(char));
   assert(charA == charB);
}

static void test_strlen(void)
{
    assert(strlen("green") == 5);
    assert(strlen("") == 0);
    assert(strlen("\0") == 0);
}

static void test_strcmp(void)
{
    assert(strcmp("apple", "apple") == 0);
    assert(strcmp("apple", "applesauce") < 0);
    assert(strcmp("pears", "apples") > 0);
    assert(strcmp("oranges", "") > 0);
    assert(strcmp("", "apples") < 0);
    assert(strcmp("", "") == 0);
}

static void test_strlcat(void)
{
    char buf[21];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    buf[0] = '\0'; // start with empty string
    assert(strlen(buf) == 0);
    strlcat(buf, "CS", sizeof(buf));
    assert(strlen(buf) == 2);
    assert(strcmp(buf, "CS") == 0);
    strlcat(buf, "107e", sizeof(buf));
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "CS107e") == 0);
    strlcat(buf, " is so much fun but its too hard", sizeof(buf));
    assert(strlen(buf) == sizeof(buf));
    assert(strcmp(buf, "CS107e is so much fun") == 0);
    
    char buf2[200];
    memset(buf2, 0x77, sizeof(buf2));
    buf2[0] = '\0';
    strlcat(buf2, "I know best where my own shoe pinches.", sizeof(buf2));
    strlcat(buf2, "Sitting quietly, doing nothing.Spring comes, and the grass grows by itself.", sizeof(buf2));
   // assert(strcmp(buf, "I know best where my own shoe pinches.Sitting quietly, doing nothing.Spring comes, and the grass grows by itself.") == 0);
    memset(buf, 0x77, sizeof(buf));
    buf[0] = '\0';
    strlcat(buf, "cs107e", sizeof(buf));
    strlcat(buf, "Wo hen kuai le.", sizeof(buf)); 
    assert(strlen(buf) == 21);
}

static void test_strtonum(void)
{
    int val = strtonum("013", NULL);
    assert(val == 13);

    const char *input = "107rocks", *rest = NULL;
    val = strtonum(input, &rest);
    assert(val == 107);
    assert(rest == &input[3]);
    const char *input2 = "rocksin107e?"; 
    val = strtonum(input2, &rest);
    assert(val == 0);
    assert(rest == &input2[0]);
    input2 = "0xef";
    val = strtonum(input2, NULL);
    assert(val == 239);
    input2 = "8812"
    val = strtonum(input2, NULL);
    assert(val == 8812);     
}

static void test_to_base(void)
{
    char buf[5];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, bufsize); // init contents with known value

    int n = signed_to_base(buf, bufsize, -9999, 10, 6);
    assert(strcmp(buf, "-099") == 0)
    assert(n == 6);

    n = signed_to_base(buf, bufsize, 1010, 10, 6);
    assert(strcmp(buf, "0010") == 0);
    assert(n == 5);
	
  //  n = signed_to_base(buf, bufsize, -9999, 16, 2);
  //  assert(strcmp(buf, "-0x2") == 0);
  //  assert(n == 6);

    n = signed_to_base(buf, bufsize, 1234, 10, 0);
    assert(strcmp(buf, "1234") == 0);
    assert(n == 5);
}

static void test_snprintf(void)
{
    char buf[100];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    // Start off simple...
    //snprintf(buf, bufsize, "You finish my %s. Or is it %s", "sandwiches", "pickles");
    //assert(strcmp(buf, "You finish my sandwiches. Or is it ?") == 0);

    // Decimal
    snprintf(buf, bufsize, "I took CS%d%d%dE %d times.", 1,0,7,0);
    assert(strcmp(buf, "I took CS107E 0 times.") == 0);

    // Hexadecimal
    snprintf(buf, bufsize, "%04x", 0xef);
    assert(strcmp(buf, "00ef") == 0);

    // Pointer
    snprintf(buf, bufsize, "%p", (void *) 0x20200004);
    assert(strcmp(buf, "0x20200004") == 0);

    // Character
    snprintf(buf, bufsize, "%c", 'A');
    assert(strcmp(buf, "A") == 0);

    // String
    snprintf(buf, bufsize, "%s", "binky");
    assert(strcmp(buf, "binky") == 0);

    // Format string with intermixed codes
    snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
    assert(strcmp(buf, "CS107e!") == 0);

    // Test return value
    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);
}


void main(void)
{
    // TODO: Add more and better tests!

    //uart_init();
    //uart_putstring("Start execute main() in tests/test_strings_printf.c\n");

    test_memset();
    test_memcpy();
    test_strlen();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_to_base();
    test_snprintf();

    //uart_putstring("Successfully finished executing main() in tests/test_strings_printf.c\n");
    //uart_putchar(EOT);
}
