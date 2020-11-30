#include "backtrace.h"
#include "printf.h"
#include "strings.h"

const char *name_of(uintptr_t fn_start_addr)
{
    if(*((unsigned int*)fn_start_addr - 1) & 0xFF000000){
	int length = (int)*((char*)fn_start_addr-4) + 4;
	return (char*)fn_start_addr - length;
    }
    return "???";
}

int backtrace (frame_t f[], int max_frames)
{
    int count = 0;
    void *fp;
    __asm__("mov %0, fp" : "=r" (fp));
    uintptr_t *lr = (uintptr_t*)((char*)fp - 4);
    uintptr_t *prev_fp_addr = (uintptr_t*)((char*)fp - 12);
    uintptr_t prev_fp = (uintptr_t)((unsigned int*)*(uintptr_t*)*prev_fp_addr - 3);
    int offset = *lr - prev_fp;
    while(count != max_frames){
	f[count].resume_addr = *lr;
	f[count].resume_offset = offset;
	f[count].name = name_of(prev_fp);
	fp = (uintptr_t*)*prev_fp_addr;
	lr = ((uintptr_t*)((char*)fp - 4));	
	prev_fp_addr = ((uintptr_t*)((char*)fp - 12));
	prev_fp = (uintptr_t)((unsigned int*)*(uintptr_t*)*prev_fp_addr - 3);
	offset = *lr - prev_fp;
	count++;
	if(fp == 0){
		f[count-1].name = "_cstart";
		break;
	}
    }
    return count;
}

void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
