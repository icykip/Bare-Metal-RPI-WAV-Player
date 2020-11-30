/*
 * LARSON SCANNER
 *
 * Currently this code is the blink code copied from lab 1.
 *
 * Modify the code to implement the larson scanner for assignment 1.
 *
 * Make sure to use GPIO pins 20-23 (or 20-27) for your scanner.
 */

.equ DELAY, 0xFF000
.equ CYCLE, 0x3F000
.equ DIM, 0x9
.equ MAX, 0x800000
.equ MIN, 0x100000

// configure GPIO 20-23 for output
ldr r0, FSEL2
mov r1, #1
add r1, #(1<<3)
add r1, #(1<<6)
add r1, #(1<<9)
str r1, [r0]

mov r1, #(1<<19)

forward: //Blink the lights in a forward direction 

lsl r1, r1, #1 //Shift bit left one to select next LED

mov r5, #CYCLE
mov r8, #DIM

loop1: //Imitate pulse width modulation with continuous loops

// set GPIO X high
ldr r0, SET0
str r1, [r0] 
subs r8, #1
beq dim
b cont

dim:

	//set prev pin on
	lsr r3, r1, #1 
	ldr r0, SET0
	str r3, [r0]

	//set next pin on
	lsl r4, r1, #1
	ldr r0, SET0
	str r4, [r0]
	
	//turn off prev pin
	ldr r0, CLR0
	str r3, [r0]

	//turn off next pin
	ldr r0, CLR0
	str r4, [r0]

	mov r8, #DIM //increment delay for dim lights

cont:

// set GPIO X low
ldr r0, CLR0
str r1,[r0]

subs r5, #1 //increment delay for current state
bne loop1

mov r2, #MAX
tst r2, r1
bne backward // If you reach the maximum value start goinmg backwards
b forward //otherwise repeat

backward: //Blink the lights backwards

lsr r1, r1, #1 //Shift bit right one to select the next LED

mov r5, #CYCLE
mov r8, #DIM

loop2: //Imitate Pulse Width Modulation with  numerous consecutive loops

// set GPIO X high
ldr r0, SET0
str r1, [r0] 
subs r8, #1//Increment delay to turn on dimmed lights
beq dim2
b cont2

dim2:

	//set prev pin on
	lsr r3, r1, #1 
	ldr r0, SET0
	str r3, [r0]

	//set next pin on
	lsl r4, r1, #1
	ldr r0, SET0
	str r4, [r0]
	
	//turn off prev pin
	ldr r0, CLR0
	str r3, [r0]

	//turn off next pin
	ldr r0, CLR0
	str r4, [r0]

	mov r8, #DIM //Reset delay for dimmed lights

cont2:

// set GPIO X low
ldr r0, CLR0
str r1,[r0]

subs r5, #1 //Increment delay for current state
bne loop2

mov r2, #MIN
tst r2, r1
bne forward // If you reach the maximum value start goinmg backwards
b backward //otherwise repeat


FSEL0: .word 0x20200000
FSEL1: .word 0x20200004
FSEL2: .word 0x20200008
SET0:  .word 0x2020001C
SET1:  .word 0x20200020
CLR0:  .word 0x20200028
CLR1:  .word 0x2020002C
