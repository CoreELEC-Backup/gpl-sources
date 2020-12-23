#ifndef _LIBMPO_DMPO_H_
#define _LIBMPO_DMPO_H_

#include <stdbool.h>
#include "mpo.h"

/**
 * \file dmpo.h
 * @brief Decompression methods and objects
 * \author Lectem
 */

typedef struct mpo_decompress mpo_decompress_struct;

typedef struct decompress_struct_extended {
    struct jpeg_decompress_struct cinfo;
    mpo_decompress_struct * mpo;
} jpeg_decompress_struct_extended ;


typedef struct mpo_decompress
{
    MPExt_Data* APP02;/*!<Array of each image data*/
    jpeg_decompress_struct_extended cinfo;
    struct jpeg_error_mgr jerr;
    //JOCTET ** images_data;/*!<Array pointing to each image data, used only when decoding the whole MPO at once*/
    int currentImage;
} mpo_decompress_struct;




/////////////////////
///////TODO//////////
/////////////////////

/** \brief Initializes the mpoinfo struct
 *
 * Call this before doing anything with the decompression struct.
 * It will initialize and create all necessary stuff for the decompression.
 * \note Please call mpo_destroy_decompress once done with the decompression struct
 */
void mpo_create_decompress(mpo_decompress_struct * mpoinfo);

/**\brief Set the source as a stdio FILE
 *
 * The file has to be opened before calling.
 * \note The file has to be closed by the caller once the decompression is finished.
 */
void mpo_stdio_src(mpo_decompress_struct * mpoinfo,FILE * input);

void mpo_mem_src(mpo_decompress_struct * mpoinfo,unsigned char * inbuffer, unsigned long insize);

/**\brief Read the first image header
 *
 * This will load all the MPO header information of the first image.
 * You need to call this once before starting any decompression
 * \note Individual images headers for the other images will be retrieved by calling mpo_start_decompress
 * \return false if suspended due to lack of input data
 * \return true if found a valid header
 */
bool mpo_read_header(mpo_decompress_struct * mpoinfo);

/**\brief Number of images of the file
 * \note Only works after reading the 1st image header
 */
int mpo_get_number_images(mpo_decompress_struct * mpoinfo);

/**\brief Start JPEG decompression of current image*/
bool mpo_start_decompress(mpo_decompress_struct * mpoinfo);

/**\brief the output colorspace */
J_COLOR_SPACE mpo_get_color_space(mpo_decompress_struct * mpoinfo);


/**\brief Next scanline to be read from jpeg_read_scanlines().
 *
 * Application may use this to control its processing loop, e.g.,
 * "while (get_output_scanline(mpoinfo) < get_output_height(mpoinfo))".
 */
JDIMENSION get_output_scanline(mpo_decompress_struct * mpoinfo);
/**\brief The output width*/
JDIMENSION get_output_width(mpo_decompress_struct * mpoinfo);
/**\brief The output height*/
JDIMENSION get_output_height(mpo_decompress_struct * mpoinfo);

/**\brief convenience function
 * \return output_scanline >= output_height
 */
bool all_scanlines_processed(mpo_decompress_struct * mpoinfo);


/**\brief Read some scanlines of data from the JPEG decompressor.
 *
 * The return value will be the number of lines actually read.
 * This may be less than the number requested in several cases,
 * including bottom of image, data source suspension, and operating
 * modes that emit multiple scanlines at a time.
 */
size_t mpo_read_scanlines (mpo_decompress_struct * mpoinfo,JSAMPARRAY scanlines, size_t max_lines);


/**\brief Skips to the beggining of an image
 *
 * You can skip data by calling this function. You can then use mpo_start_decompress to resume decompression of the image passed in arguments.
 * \note The skipped images header information retrieval will also be skipped.
 */
void mpo_skip_to_image(mpo_decompress_struct * mpoinfo,int image);

/**\brief Finish JPEG decompression.
 *
 * This will also set currentImage to currentImage+1
 */
bool mpo_finish_decompress(mpo_decompress_struct * mpoinfo);

/**\brief Destroy the mpoinfo struct. */
void mpo_destroy_decompress(mpo_decompress_struct * mpoinfo);


/**\brief Function called when an error occurs in libjpeg.
 *
 * Defaults to stopping the application
 */
void mpo_decompress_error_exit(mpo_decompress_struct *mpoinfo,void exit_func (j_common_ptr));

#endif //_LIBMPO_DMPO_H_
