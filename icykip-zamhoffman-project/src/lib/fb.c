#include "fb.h"
#include "mailbox.h"

typedef struct {
    unsigned int width;       // width of the physical screen
    unsigned int height;      // height of the physical screen
    unsigned int virtual_width;  // width of the virtual framebuffer
    unsigned int virtual_height; // height of the virtual framebuffer
    unsigned int pitch;       // number of bytes per row
    unsigned int bit_depth;   // number of bits per pixel
    unsigned int x_offset;    // x of the upper left corner of the virtual fb
    unsigned int y_offset;    // y of the upper left corner of the virtual fb
    unsigned int framebuffer; // pointer to the start of the framebuffer
    unsigned int total_bytes; // total number of bytes in the framebuffer
} fb_config_t;

// fb is volatile because the GPU will write to it
static volatile fb_config_t fb __attribute__ ((aligned(16)));

void fb_init(unsigned int width, unsigned int height, unsigned int depth_in_bytes, fb_mode_t mode)
{
    // TODO: extend this function to support double-buffered mode
    fb.width = width;
    fb.virtual_width = width;
    fb.height = height;
    if(mode == FB_SINGLEBUFFER) fb.virtual_height = height;
    else fb.virtual_height = 2 * height;
    fb.bit_depth = depth_in_bytes * 8; // convert number of bytes to number of bits
    fb.x_offset = 0;
    fb.y_offset = 0;

    // the manual requires we to set these value to 0
    // the GPU will return new values
    fb.pitch = 0;
    fb.framebuffer = 0;
    fb.total_bytes = 0;

    // Send address of fb struct to the GPU
    mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned int)&fb);
    // Read response from GPU
    mailbox_read(MAILBOX_FRAMEBUFFER);
}


void fb_swap_buffer(void)
{
    if(fb.virtual_height == fb.height) return;
    if(fb.y_offset == 0) fb.y_offset = fb.height;
    else fb.y_offset = 0;
    
    mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned int)&fb);
    mailbox_read(MAILBOX_FRAMEBUFFER);
}

void* fb_get_draw_buffer(void)
{
    if(fb.virtual_height == fb.height) return (void*)fb.framebuffer;
    else{
	if(fb.y_offset == fb.height) return (void*)fb.framebuffer;
	else return ((char*)fb.framebuffer + (fb.height*fb.pitch));
    }
}

unsigned int fb_get_width(void)
{
    return fb.width;
}

unsigned int fb_get_height(void)
{
    return fb.height;
}

unsigned int fb_get_depth(void)
{
    return fb.bit_depth/8;
}

unsigned int fb_get_pitch(void)
{
    return fb.pitch;
}

