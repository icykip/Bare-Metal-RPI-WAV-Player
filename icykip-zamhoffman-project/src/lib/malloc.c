/*
 * File: malloc.c
 * --------------
 * This is the simple "bump" allocator from lecture.
 * An allocation request is serviced by using sbrk to extend
 * the heap segment. 
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include "malloc_internal.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"

extern int __bss_end__;

#define STACK_START 0x8000000
#define STACK_SIZE  0x1000000
#define STACK_END ((char *)STACK_START - STACK_SIZE)

/*
 * The pool of memory available for the heap starts at the upper end of the 
 * data section and can extend from there up to the lower end of the stack.
 * It uses symbol __bss_end__ from memmap to locate data end
 * and calculates stack end assuming a 16MB stack.
 *
 * Global variables for the bump allocator:
 *
 * `heap_start`  location where heap segment starts
 * `heap_end`    location at end of in-use portion of heap segment 
 */

// Initial heap segment starts at bss_end and is empty
static void *heap_start = &__bss_end__;
static void *heap_end = &__bss_end__;
	 
struct header {
    size_t payload_size;
    int status;       // 0 if free, 1 if in use
};

void *sbrk(int nbytes)
{
    void *prev_end = heap_end;
    if ((char *)prev_end + nbytes > STACK_END) {
        return NULL;
    } else {
        heap_end = (char *)prev_end + nbytes;
        return prev_end;
    }
}


// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

void *malloc (size_t nbytes)
{
    nbytes = roundup(nbytes, 8);
    struct header head;	
    struct header * stepper = heap_start;
    int size = 0;
    while(stepper != heap_end){
	size = (*stepper).payload_size;
	if(size > nbytes + 8 && (*stepper).status == 0){
		head.payload_size = size - nbytes - 8;
		head.status = 0;
		*(stepper + nbytes/8 + 1) = head;
		size = nbytes;
		break;
	}
	else if(size == nbytes && (*stepper).status == 0)
		break;
	stepper += size/8 + 1;
    }
    head.payload_size = nbytes;
    head.status = 1;
    *stepper = head;
    if(heap_end == stepper) sbrk(nbytes + 8);
    return stepper + 1;
}

void free (void *ptr)
{
    struct header * free_me = ptr;
    free_me = free_me - 1;
    (*free_me).status = 0;
    struct header * new_free = free_me + (*free_me).payload_size/8 + 1;
    while((*new_free).status  == 0 && (int)new_free < (int)heap_end){
	(*free_me).payload_size += (*new_free).payload_size + 8;
	new_free += (*new_free).payload_size/8 + 1;
    }
}

void *realloc (void *orig_ptr, size_t new_size)
{
    // TODO: replace with your code
    struct header * find_space = orig_ptr;
    find_space -= 1;
    unsigned int avail_size = (*find_space).payload_size;
    while(avail_size < new_size && find_space != heap_end){
	find_space += (*find_space).payload_size/8 + 1;
	if((*find_space).status != 0) break;
	avail_size += (*find_space).payload_size + 8;
    }
    if(avail_size >= new_size){
	find_space = orig_ptr;
	find_space -= 1;
	(*find_space).payload_size = avail_size;
	return orig_ptr;
    }	 
    void *new_ptr = malloc(new_size);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, orig_ptr, new_size);
    free(orig_ptr);
    return new_ptr;
}

void heap_dump (const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", heap_start, heap_end);
    struct header * ptr = heap_start;
    while((int)ptr < (int)heap_end){
 	printf("%p status: %d --- size: %d\n", ptr, (*ptr).status, (*ptr).payload_size);
	ptr = (struct header*)((char*)ptr + (*ptr).payload_size + 8);
    }
    printf("----------  END DUMP (%s) ----------\n", label);
}

void memory_report (void)
{
    printf("\n=============================================\n");
    printf(  "         Mini-Valgrind Memory Report         \n");
    printf(  "=============================================\n");
    // TODO: fill in for extension
}
