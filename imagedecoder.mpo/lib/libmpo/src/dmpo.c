#include <wchar.h>
#include <jpeglib.h>
#include <search.h>
#include <string.h>
#include <libmpo/dmpo.h>
#include <libmpo/mpo.h>
#include <libmpo/mpodatasrc.h>
#include <assert.h>
#include "libmpo/mpo.h"
#include "libmpo/dmpo.h"


unsigned int jpeg_getc (j_decompress_ptr cinfo)
/* Read next byte */
{
    assert(cinfo != 0);
    struct jpeg_source_mgr * datasrc = cinfo->src;

    if (datasrc->bytes_in_buffer == 0)
    {
        if (! (*datasrc->fill_input_buffer) (cinfo))
            exit(-1);//ERREXIT(cinfo, JERR_CANT_SUSPEND);
    }
    datasrc->bytes_in_buffer--;
    return GETJOCTET(*datasrc->next_input_byte++);
}


boolean MPExtReadAPP02 (j_decompress_ptr cinfo)
{
    int i=0;

    int currentImage = ((jpeg_decompress_struct_extended*)cinfo)->mpo->currentImage;
    MPExt_Data *data=&((jpeg_decompress_struct_extended*)cinfo)->mpo->APP02[currentImage];
    memset(data,0,sizeof *data);
    int length;
    length = jpeg_getc((j_decompress_ptr)cinfo) << 8;
    length += jpeg_getc((j_decompress_ptr)cinfo);
    mpo_printf( "APP02, length %d:\n",length);
    length -= 2;
    for(i=0; i<4; ++i)data->MPF_identifier[i]=jpeg_getc(cinfo);
    length-=4;
    if(data->MPF_identifier[0] != 'M'||
            data->MPF_identifier[1] != 'P'||
            data->MPF_identifier[2] != 'F'||
            data->MPF_identifier[3] != 0)
    {
        /*Ignore this block*/
        while(length-- >0)
        {
            jpeg_getc(cinfo);
        }
        return 1;
    }
    ((jpeg_decompress_struct_extended*)cinfo)->mpo->APP02[currentImage].start_of_offset = my_ftell((my_src_ptr) cinfo->src);
    printf("Start of offset at position 0x%x of file\n",((jpeg_decompress_struct_extended*)cinfo)->mpo->APP02->start_of_offset);
    MPFbuffer buf;
    buf.buffer  = calloc(length,sizeof(MPFByte));
    buf._cur=0;
    buf._size=length;
    for(i=0;i<length;i++)
    {
        buf.buffer[i]=jpeg_getc(cinfo);
    }

    int ret= MPExtReadMPF(&buf,data,!currentImage);
    return ret;
}




void mpo_create_decompress(mpo_decompress_struct *mpoinfo) {
    if(mpoinfo) {
        memset(mpoinfo, 0, sizeof *mpoinfo);
        mpoinfo->APP02 = calloc(1, sizeof *mpoinfo->APP02);
        mpoinfo->cinfo.mpo = mpoinfo;
        ((j_decompress_ptr) &mpoinfo->cinfo)->err = jpeg_std_error(&mpoinfo->jerr);
        jpeg_create_decompress((j_decompress_ptr) &mpoinfo->cinfo);
    }
}


void mpo_destroy_decompress(mpo_decompress_struct *mpoinfo) {
    int i;
    int nbImages = 0;
    if(mpoinfo) {
        if (mpoinfo->APP02) {
            nbImages =mpoinfo->APP02->numberOfImages;
            for(i=0;i<nbImages;++i)
            {
                destroyMPF_Data(&mpoinfo->APP02[i]);
            }
            free(mpoinfo->APP02);
            mpoinfo->APP02 = NULL;
        }
        jpeg_destroy_compress((j_compress_ptr) &mpoinfo->cinfo);

        /*if (mpoinfo->images_data)free(mpoinfo->images_data);
        mpoinfo->images_data = NULL;*/
    }
}


void mpo_stdio_src(mpo_decompress_struct *mpoinfo, FILE *input) {
    if(mpoinfo && input && ftell(input) != -1L)
        my_jpeg_stdio_src((j_decompress_ptr)&mpoinfo->cinfo,input);
}


void mpo_mem_src(mpo_decompress_struct *mpoinfo, unsigned char *inbuffer, unsigned long insize) {
    if(mpoinfo && inbuffer && insize > 0 )
        my_jpeg_mem_src((j_decompress_ptr)&mpoinfo->cinfo,inbuffer,insize);
}


bool mpo_read_header(mpo_decompress_struct *mpoinfo) {
    bool res = false;
    if(mpoinfo) {
        mpoinfo->currentImage = 0;
        jpeg_set_marker_processor((j_decompress_ptr) &mpoinfo->cinfo, JPEG_APP0 + 2, MPExtReadAPP02);

        res = jpeg_read_header((j_decompress_ptr) &mpoinfo->cinfo, TRUE) != 0;
        int nbImages = mpoinfo->APP02->numberOfImages;
        if(nbImages > 1)
            mpoinfo->APP02 = realloc(mpoinfo->APP02,nbImages * (sizeof *mpoinfo->APP02));

    }
    return res;
}

int mpo_get_number_images(mpo_decompress_struct *mpoinfo) {
    if(mpoinfo && mpoinfo->APP02) return mpoinfo->APP02->numberOfImages;
    else return 0;
}

J_COLOR_SPACE mpo_get_color_space(mpo_decompress_struct *mpoinfo) {
    return mpoinfo->cinfo.cinfo.out_color_space;
}

bool mpo_start_decompress(mpo_decompress_struct *mpoinfo) {
    if(mpoinfo)
    {
        if(mpoinfo->currentImage == 0)
        {
            jpeg_start_decompress((j_decompress_ptr) &mpoinfo->cinfo);
        }
        if(mpoinfo->currentImage >0)
        {
            my_src_ptr src_ptr = (my_src_ptr) mpoinfo->cinfo.cinfo.src;
            size_t curPos = my_ftell(src_ptr);
            mpo_printf("skip %d bytes (to the beggining of image %d)\n",(mpoinfo->APP02->MPentry[mpoinfo->currentImage].offset+mpoinfo->APP02->start_of_offset) - curPos,mpoinfo->currentImage);
            src_ptr->pub.skip_input_data(
                    &mpoinfo->cinfo.cinfo,
                    (mpoinfo->APP02->MPentry[mpoinfo->currentImage].offset+mpoinfo->APP02->start_of_offset) - curPos
            );
            size_t posAfterSkip = my_ftell(src_ptr);
            mpo_printf("Image %d SOI offset : %d(0x%x)\n",mpoinfo->currentImage,posAfterSkip,posAfterSkip);
            //TODO:go to the good position
            jpeg_set_marker_processor((j_decompress_ptr) &mpoinfo->cinfo, JPEG_APP0 + 2, MPExtReadAPP02);

            jpeg_read_header((j_decompress_ptr) &mpoinfo->cinfo,TRUE);//Move this somewhere else?
            jpeg_start_decompress((j_decompress_ptr) &mpoinfo->cinfo);
        }
    }

    return false;
}


JDIMENSION get_output_scanline(mpo_decompress_struct *mpoinfo) {
    return mpoinfo->cinfo.cinfo.output_scanline;
}

JDIMENSION get_output_width(mpo_decompress_struct *mpoinfo) {
    return mpoinfo->cinfo.cinfo.output_width;
}

JDIMENSION get_output_height(mpo_decompress_struct *mpoinfo) {
    return mpoinfo->cinfo.cinfo.output_height;
}

bool all_scanlines_processed(mpo_decompress_struct *mpoinfo) {
    return mpoinfo->cinfo.cinfo.output_scanline >= mpoinfo->cinfo.cinfo.output_height ;
}

size_t mpo_read_scanlines(mpo_decompress_struct *mpoinfo, JSAMPARRAY scanlines, size_t max_lines) {
    return jpeg_read_scanlines((j_decompress_ptr) &mpoinfo->cinfo,scanlines,max_lines);
}


void mpo_skip_to_image(mpo_decompress_struct *mpoinfo, int image) {
    if(mpoinfo && mpoinfo->currentImage < image) {
        my_src_ptr src_ptr = (my_src_ptr) mpoinfo->cinfo.cinfo.src;
        size_t curPos = my_ftell(src_ptr);
        mpo_printf("skip %d bytes (to the beggining of image %d)\n",
                   (mpoinfo->APP02->MPentry[image].offset + mpoinfo->APP02->start_of_offset) - curPos,
                   image);
        src_ptr->pub.skip_input_data(
                &mpoinfo->cinfo.cinfo,
                (mpoinfo->APP02->MPentry[image].offset + mpoinfo->APP02->start_of_offset) - curPos
        );
        size_t posAfterSkip = my_ftell(src_ptr);
        mpoinfo->currentImage=image;
        mpo_printf("Image %d SOI offset : %d(0x%x)\n", image, posAfterSkip, posAfterSkip);
    }
}

bool mpo_finish_decompress(mpo_decompress_struct *mpoinfo) {
    if(mpoinfo)
    {
        jpeg_finish_decompress((j_decompress_ptr) &mpoinfo->cinfo);
        mpoinfo->currentImage++;
    }
    return false;
}

void mpo_decompress_error_exit(mpo_decompress_struct *mpoinfo,void exit_func (j_common_ptr)){
    mpoinfo->jerr.error_exit = exit_func;
}
