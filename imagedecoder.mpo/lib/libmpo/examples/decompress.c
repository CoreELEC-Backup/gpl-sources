//
// Created by Lectem on 12/03/2015.
//

#include <libmpo/dmpo.h>
#include <stdio.h>
#include "decompress.h"

void decompress_mpo(char *);

int main(int argc, char * argv[])
{
    decompress_mpo("3DS-big-endian.mpo");
    decompress_mpo("test-little-endian.mpo");
    return 0;
}


void decompress_mpo(char * filename)
{

    printf( "=======================================\n"
            "=======DECOMPRESS MPO FROM FILE========\n"
            "=======================================\n");

    FILE * f = fopen(filename,"rb");
    if(f)
    {
        printf("Opened file %s\n",filename);
        mpo_decompress_struct mpoinfo;
        mpo_create_decompress(&mpoinfo);
        mpo_stdio_src(&mpoinfo,f);
        mpo_read_header(&mpoinfo);
        while(mpoinfo.currentImage < 2)//mpo_get_number_images(&mpoinfo))
        {
            mpo_start_decompress(&mpoinfo);
            JSAMPARRAY buffer;
            int row_stride = mpoinfo.cinfo.cinfo.output_width * mpoinfo.cinfo.cinfo.output_components;
            while(!all_scanlines_processed(&mpoinfo))
            {

                buffer = (*mpoinfo.cinfo.cinfo.mem->alloc_sarray)
                        ((j_common_ptr) &mpoinfo.cinfo, JPOOL_IMAGE, row_stride, 1);
                mpo_read_scanlines(&mpoinfo,buffer,1);
            }
            mpo_finish_decompress(&mpoinfo);
        }

        mpo_destroy_decompress(&mpoinfo);
        fclose(f);
    }
    else
        printf("Couldn't open file %s\n",filename);
}