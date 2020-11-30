.equ DELAY, 0xFF000
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

forward: 
lsl r1, r1, #1 //Shift bit left one to select next LED

// set GPIO X high
ldr r0, SET0
str r1, [r0] 

// delay
mov r6, #DELAY
wait1:
    subs r6, #1
    bne wait1
   
// set GPIO X low
ldr r0, CLR0
str r1,[r0]

mov r2, #MAX
tst r2, r1
bne backward // If you reach the maximum value start goinmg backwards
b forward //otherwise repeat

backward: 

lsr r1, r1, #1 //Shift bit right one to select the next LED

// set GPIO X high
ldr r0, SET0
str r1, [r0] 

// delay
mov r6, #DELAY
wait2:
    subs r6, #2
    bne wait2

// set GPIO X low
ldr r0, CLR0
str r1,[r0]

mov r2, #MIN
tst r2, r1 
bne forward //If you reach the minimum value start going forwards
b backward //otherwise repeate

FSEL0: .word 0x20200000
FSEL1: .word 0x20200004
FSEL2: .word 0x20200008
SET0:  .word 0x2020001C
SET1:  .word 0x20200020
CLR0:  .word 0x20200028
