/**
 * \file
 * \brief
 * \author Lectem
 */


#pragma once


void mpo_init_compress(mpo_compress_struct* mpoinfo,int numberOfImages);

void mpo_destroy_compress(mpo_compress_struct* mpoinfo);

/** \brief Tell mpoinfo struct what source to use
 *
 * \param imageNumber   The index of the image we are working on
 * \param src          An array of data, shall be freed by the user
 *
 */
void mpo_image_mem_src(mpo_compress_struct* mpoinfo,int imageNumber,JOCTET src[]);

/** \brief Convenience function to set width and height of all images at once
*/
void mpo_dimensions_forall(mpo_compress_struct* mpoinfo,int width,int height);

/** \brief Convenience function to set colorspace info of all images at once
*/
void mpo_colorspace_forall(mpo_compress_struct* mpoinfo,J_COLOR_SPACE jcs,int input_components);

/** \brief Convenience function to set quality of all images at once
*/
void mpo_quality_forall(mpo_compress_struct* mpoinfo,int quality);

/** \brief Convenience function to set type of all images at once
*/
void mpo_type_forall(mpo_compress_struct* mpoinfo,MPExt_MPType type);


GLOBAL(void)
mpo_write_file (mpo_compress_struct* mpoinfo,char * filename);

void mpo_write_MPO_Marker(mpo_compress_struct * mpoinfo,int image);

void mpo_init_3d_compress(mpo_compress_struct* mpoinfo,JOCTET *imageBytes_left,JOCTET *imageBytes_right,int image_width,int image_height);
