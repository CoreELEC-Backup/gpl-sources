#include <stdio.h>
#include <stdlib.h>
#include <libmpo/mpo.h>
#include "libmpo/mpo.h"
#include "libmpo/cmpo.h"

/*Images data generated with Image magick convert (convert file.jpg file.rgb)
* Then converted to raw array with GNU bin2c (bin2c file.rgc > fileraw.c)
*/
extern unsigned char imageBytes_left[];
extern unsigned char imageBytes_right[];

int main()
{
    //decompress_mpo("3DS-big-endian.mpo");
    //decompress_mpo("test-little-endian.mpo");



    int image_width=640;
    int image_height=480;
    mpo_compress_struct mpoinfo;
    mpo_init_compress(&mpoinfo,2);
    mpo_quality_forall(&mpoinfo,75);
    mpo_dimensions_forall(&mpoinfo,image_width,image_height);
    mpo_colorspace_forall(&mpoinfo,JCS_RGB,3);

    mpo_image_mem_src(&mpoinfo,0,imageBytes_left);
    mpo_image_mem_src(&mpoinfo,1,imageBytes_right);
    mpo_type_forall(&mpoinfo,MPType_MultiFrame_Disparity);//This is the type for 3D images

    mpo_write_file(&mpoinfo,"test.mpo");
    //Writes the mpo file WITHOUT exif
    mpo_destroy_compress(&mpoinfo);
    /*test.mpo can be read with any standard compliant viewer*/

    /*To view it with your 3DS you have to put it in the /DCIM/100NIN03 folder*/
    /*or any folder your 3DS pictures are saved at. Then rename test.mpo to HNI_xxxx.MPO, depending on what number is available*/
    /*3DS viewer can only display pictures of 640x480 pixels*/


    //We will be able to test it once the decompression will be done correctly
    //Actually not working because of the tricky SOI+EXIF markers lookup
    //decompress_mpo("test.mpo");

    return 0;
}
