//
// Created by Lectem on 10/04/2015.
//

#ifndef LIBMPO_MPODATASRC_H
#define LIBMPO_MPODATASRC_H

#include <stdio.h>
#include "jpeglib.h"
#include "jerror.h"


typedef struct {
    struct jpeg_source_mgr pub;	/* public fields */

    FILE * infile;		/* source stream */
    JOCTET * buffer;		/* start of buffer */
    boolean start_of_file;	/* have we gotten any data yet? */
    size_t buffer_file_offset;
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;


GLOBAL(void)
my_jpeg_stdio_src (j_decompress_ptr cinfo, FILE * infile);

GLOBAL(void)
my_jpeg_mem_src (j_decompress_ptr cinfo, unsigned char * inbuffer, unsigned long insize);

static size_t my_ftell(my_src_ptr src)
{
    if(src->pub.next_input_byte)
    {
        return src->buffer_file_offset + (src->pub.next_input_byte - src->buffer);
    }
    else return src->buffer_file_offset;
}

#endif //LIBMPO_MPODATASRC_H
