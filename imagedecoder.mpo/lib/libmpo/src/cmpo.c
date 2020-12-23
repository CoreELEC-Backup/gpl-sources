#include "libmpo/mpo.h"
#include "libmpo/cmpo.h"

#include <assert.h>
#include <libmpo/mpo.h>

typedef struct
{
    struct jpeg_destination_mgr pub; /* public fields */

    FILE * outfile;		/* target stream */
    JOCTET * buffer;		/* start of buffer */
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;
#define OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */




void mpo_init_write(MPExt_Data * data)
{
    /*Everything should be set to when entering here*/
    data->MPF_identifier[0]='M';
    data->MPF_identifier[1]='P';
    data->MPF_identifier[2]='F';
    data->MPF_identifier[3]=0;
    if(isLittleEndian())
        data->byte_order= MPF_LITTLE_ENDIAN;
    else    data->byte_order= MPF_BIG_ENDIAN;

    data->first_IFD_offset=8;

    data->version[0]='0';
    data->version[1]='1';
    data->version[2]='0';
    data->version[3]='0';


    data->count=3;//Number of tags
    data->EntryIndex.type=07;
    data->EntryIndex.EntriesTabLength=data->numberOfImages*16;
    //data->EntryIndex.dataOffset=50/*End of MPentry*/
    //                                  +0*12/*Individual unique ID List ?*/
    //                                  +0*12/*Total number of captured frames*/;
    //data->nextIFDOffset=data->EntryIndex.dataOffset
    //                    +data->EntryIndex.EntriesTabLength/*Starts right after Value Index IFD*/;
}


void mpo_init_compress(mpo_compress_struct* mpoinfo,int numberOfImages)
{
    int i;
    //could use memset, but would need to include string.h
    for(i=0;i<sizeof(*mpoinfo);++i)
    {
        ((char*)mpoinfo)[i]=0;
    }
    mpoinfo->images_data=calloc(numberOfImages,sizeof(JOCTET*));
    mpoinfo->APP02=calloc(numberOfImages,sizeof(MPExt_Data));
    mpoinfo->APP02[0].numberOfImages=numberOfImages;
    mpoinfo->APP02[0].MPentry=calloc(numberOfImages,sizeof(MPExt_MPEntry));
    mpoinfo->cinfo=calloc(numberOfImages,sizeof(struct jpeg_compress_struct));
    if(mpoinfo->cinfo)
        for(i=0; i<numberOfImages; ++i)
        {
            jpeg_create_compress(&mpoinfo->cinfo[i]);
        }
    for (i = 0; i < numberOfImages; ++i) {
        mpo_init_write(&mpoinfo->APP02[i]);
    }
    mpo_type_forall(mpoinfo,MPType_Baseline);
    if(numberOfImages>=1)mpoinfo->APP02[0].MPentry[0].individualImgAttr.data.representativeImage=1;

}


void mpo_destroy_compress(mpo_compress_struct* mpoinfo)
{
    unsigned int i;
    unsigned int numberOfImages=0;
    if(mpoinfo->APP02)
    {
        numberOfImages=mpoinfo->APP02[0].numberOfImages;
        if(mpoinfo->APP02[0].MPentry)free(mpoinfo->APP02[0].MPentry);
        mpoinfo->APP02[0].MPentry=NULL;
        free(mpoinfo->APP02);
        mpoinfo->APP02=NULL;
    }
    if(mpoinfo->cinfo)
        for(i=0; i<numberOfImages; ++i)
        {
            jpeg_destroy_compress(&mpoinfo->cinfo[i]);
        }
    if(mpoinfo->cinfo)free(mpoinfo->cinfo);
    mpoinfo->cinfo=NULL;


    if(mpoinfo->images_data)free(mpoinfo->images_data);
    mpoinfo->images_data=NULL;
}

/** \brief Tell mpoinfo struct what source to use
 *
 * \param imageNumber   The index of the image we are working on
 * \param data          An array of data, shall be freed by the user
 * \param width         Image's width in pixels
 * \param height        Image's height in pixels
 *
 */

void mpo_image_mem_src(mpo_compress_struct* mpoinfo,int imageNumber,JOCTET src[])
{
    mpoinfo->images_data[imageNumber]=src;
}

/** \brief Convenience function to set width and height of all images at once
*/
void mpo_dimensions_forall(mpo_compress_struct* mpoinfo,int width,int height)
{
    unsigned int i;
    for(i=0; i<mpoinfo->APP02[0].numberOfImages; ++i)
    {
        mpoinfo->cinfo[i].image_width=width;
        mpoinfo->cinfo[i].image_height=height;
    }
}

/** \brief Convenience function to set colorspace info of all images at once
*/
void mpo_colorspace_forall(mpo_compress_struct* mpoinfo,J_COLOR_SPACE jcs,int input_components)
{
    unsigned int i;
    for(i=0; i<mpoinfo->APP02[0].numberOfImages; ++i)
    {
        mpoinfo->cinfo[i].input_components = input_components;		/* # of color components per pixel */
        mpoinfo->cinfo[i].in_color_space = jcs; 	/* colorspace of input image */
    }
}


/** \brief Convenience function to set quality of all images at once
*/
void mpo_quality_forall(mpo_compress_struct* mpoinfo,int quality)
{
    unsigned int i;
    for(i=0; i<mpoinfo->APP02[0].numberOfImages; ++i)
    {
        jpeg_set_quality(&mpoinfo->cinfo[i], quality, TRUE );
    }
}

/** \brief Convenience function to set type of all images at once
*/
void mpo_type_forall(mpo_compress_struct* mpoinfo,MPExt_MPType type)
{

    unsigned int i;
    for(i=0; i<mpoinfo->APP02[0].numberOfImages; ++i)
    {
        mpoinfo->APP02[0].MPentry[i].individualImgAttr.data.MPTypeCode=type;
    }
}


#define IFD_VALUE_BUFFER_SIZE 1000
MPFByte ifd_value_buffer[IFD_VALUE_BUFFER_SIZE];
//index of next value
unsigned int ifd_value_buffer_current=0;
//offset pointing to the beginning of the ifd values
unsigned int ifd_value_buffer_offset=0;


void ifd_buffer_write_m_byte(MPFByte value)
{
    if(ifd_value_buffer_current<IFD_VALUE_BUFFER_SIZE) {
        ifd_value_buffer[ifd_value_buffer_current] = value;
        ifd_value_buffer_current++;
    }
}

void ifd_buffer_write_m_int16(uint16_t value)
{
    ifd_buffer_write_m_byte(value&0x00FFu);
    ifd_buffer_write_m_byte(value>>8 &0x00FFu);
}

void ifd_buffer_write_m_int32(uint32_t value)
{
    ifd_buffer_write_m_byte(value&0x000000FFu);
    ifd_buffer_write_m_byte((value>>8) &0x000000FFu);
    ifd_buffer_write_m_byte((value>>16) &0x000000FFu);
    ifd_buffer_write_m_byte((value>>24) &0x000000FFu);
}

void ifd_buffer_m_bytes(MPFByte *value,unsigned int length)
{
    unsigned int i;
    for(i=0; i<length; ++i)
        ifd_buffer_write_m_byte(value[i]);
}

int write_ifd_buffer(j_compress_ptr cinfo)
{
    unsigned int i;
    for ( i = 0; i < ifd_value_buffer_current; ++i) {
        jpeg_write_m_byte(cinfo,ifd_value_buffer[i]);
    }
    ifd_value_buffer_current=0;
    return i;
}


void jpeg_write_m_int16(j_compress_ptr cinfo,uint16_t value)
{
    jpeg_write_m_byte(cinfo,value&0x00FF);
    jpeg_write_m_byte(cinfo,value>>8 &0x00FF);
}

void jpeg_write_m_int32(j_compress_ptr cinfo,uint32_t value)
{
    jpeg_write_m_byte(cinfo,value&0x000000FF);
    jpeg_write_m_byte(cinfo,(value>>8) &0x000000FF);
    jpeg_write_m_byte(cinfo,(value>>16) &0x000000FF);
    jpeg_write_m_byte(cinfo,(value>>24) &0x000000FF);
}

void jpeg_write_m_bytes(j_compress_ptr cinfo,MPFByte *value,unsigned int length)
{
    unsigned int i;
    for(i=0; i<length; ++i)
        jpeg_write_m_byte(cinfo,value[i]);
}


int mpo_write_m_UNDEFINED(j_compress_ptr cinfo, MPFByte *value, int count)
{
    int i;
    jpeg_write_m_int16(cinfo,MPF_UNDEFINED);
    jpeg_write_m_int32(cinfo,count);
    if(count*sizeof(*value)>4)
    {
        jpeg_write_m_int32(cinfo,ifd_value_buffer_offset+ifd_value_buffer_current);
        for(i=0; i<count; ++i)
            ifd_buffer_write_m_byte(value[i]);
        return 2+4+4;
    }
    else
    {
        for(i=0; i<count; ++i)
            jpeg_write_m_byte(cinfo,value[i]);
        return 2+4+count;
    }
}



int mpo_write_m_LONG(j_compress_ptr cinfo, MPFLong *value, int count)
{
    int i;
    jpeg_write_m_int16(cinfo,MPF_LONG);
    jpeg_write_m_int32(cinfo,count);
    if(count>1)
    {
        jpeg_write_m_int32(cinfo,ifd_value_buffer_offset+ifd_value_buffer_current);
        for (i = 0; i < count; ++i)
            ifd_buffer_write_m_int32(value[i]);
    }
    else
    {
            jpeg_write_m_int32(cinfo, value[0]);
    }
    return 2 + 4 + 4 * count;
}


//FIXME : Fucking wrong ! it has to be the offset
int mpo_write_m_RATIONAL(j_compress_ptr cinfo, MPFRational *value, int count)
{
    int i;
    jpeg_write_m_int16(cinfo,MPF_RATIONAL);
    jpeg_write_m_int32(cinfo,count);

    jpeg_write_m_int32(cinfo,ifd_value_buffer_offset+ifd_value_buffer_current);
    for(i=0; i<count; ++i)
    {
        ifd_buffer_write_m_int32(value[i].numerator);
        ifd_buffer_write_m_int32(value[i].denominator);
    }
    return 0;
}

//FIXME : Fucking wrong ! it has to be the offset
int mpo_write_m_SRATIONAL(j_compress_ptr cinfo, MPFSRational *value, int count)
{
    int i;
    jpeg_write_m_int16(cinfo,MPF_SRATIONAL);
    jpeg_write_m_int32(cinfo,count);
    jpeg_write_m_int32(cinfo,ifd_value_buffer_offset+ifd_value_buffer_current);
    for(i=0; i<count; ++i)
    {
        ifd_buffer_write_m_int32(value[i].numerator);
        ifd_buffer_write_m_int32(value[i].denominator);
    }
    return 0;
}


int mpo_write_MPExtTag (j_compress_ptr cinfo,MPExt_Data *data,MPExt_MPTags tag)
{
    jpeg_write_m_int16(cinfo,tag);
    int bytes_written=2;

    switch(tag)
    {
    case MPTag_MPFVersion :
        bytes_written+= mpo_write_m_UNDEFINED(cinfo, (unsigned char *) data->version, 4);
        break;
    case MPTag_NumberOfImages :
        bytes_written+= mpo_write_m_LONG(cinfo, &data->numberOfImages, 1);
        break;
    case MPTag_MPEntry:
        jpeg_write_m_int16(cinfo,data->EntryIndex.type);/*Type 07 = undefined*/
        jpeg_write_m_int32(cinfo,data->EntryIndex.EntriesTabLength);
        data->EntryIndex.dataOffset =ifd_value_buffer_offset;
        jpeg_write_m_int32(cinfo,ifd_value_buffer_offset);
        bytes_written+=2+4+4;
        for(data->currentEntry=0; data->currentEntry < data->numberOfImages; data->currentEntry++)
        {
            ifd_buffer_write_m_int32(data->MPentry[data->currentEntry].individualImgAttr.value);
            ifd_buffer_write_m_int32(data->MPentry[data->currentEntry].size);
            ifd_buffer_write_m_int32(data->MPentry[data->currentEntry].offset);
            ifd_buffer_write_m_int16(data->MPentry[data->currentEntry].dependentImageEntry1);
            ifd_buffer_write_m_int16(data->MPentry[data->currentEntry].dependentImageEntry2);
        }
        break;
    /*Non mandatory*/
    default:
        if(tag >>8 == 0xB0)
            mpo_printf("----------------Ignoring Index IFD TAG : 0x%x----------------\n",tag);
        else if(tag>>8 == 0xB1)
            mpo_printf("-------------Ignoring Individual IFD TAG : 0x%x--------------\n",tag);
        else if(tag>>8 == 0xB2)
            mpo_printf("----------------Ignoring Attr IFD TAG : 0x%x-----------------\n",tag);
        else
            mpo_printf("-----------------------Unknown TAG : 0x%x--------------------\n",tag);
        break;
    }
    return bytes_written;
}


int mpo_write_MPExt_IndexIFD (mpo_compress_struct * mpoinfo)
{
    ifd_value_buffer_current=0;
    // IFD_OFFSET + sizeof(Count) + TAGS + offset of next ifd
    ifd_value_buffer_offset=mpoinfo->APP02[0].first_IFD_offset+2+mpoinfo->APP02[0].count*12+4;
    int bytes_written=0;
    j_compress_ptr cinfo=&mpoinfo->cinfo[0];

    //TODO : compute the number of tags
    assert(mpoinfo->APP02[0].count == 3);
    jpeg_write_m_int16(cinfo,mpoinfo->APP02[0].count);
    mpo_printf("mpoinfo->APP02[0].count=%d\n",mpoinfo->APP02[0].count);

    bytes_written+=2;
    bytes_written+=mpo_write_MPExtTag(cinfo,&mpoinfo->APP02[0],MPTag_MPFVersion);
    bytes_written+=mpo_write_MPExtTag(cinfo,&mpoinfo->APP02[0],MPTag_NumberOfImages);
    bytes_written+=mpo_write_MPExtTag(cinfo,&mpoinfo->APP02[0],MPTag_MPEntry);
    mpoinfo->APP02[0].nextIFDOffset=ifd_value_buffer_offset+ifd_value_buffer_current;
    jpeg_write_m_int32(cinfo,mpoinfo->APP02[0].nextIFDOffset);
    bytes_written+=4;
    bytes_written+=write_ifd_buffer(cinfo);
    return bytes_written;
}

int mpo_write_MPExt_AttrIFD(mpo_compress_struct * mpoinfo, int i)
{
    ifd_value_buffer_current=0;
    // offset of the attrIFD + size of tags (all tags of attrifd take 12bytes)
    ifd_value_buffer_offset=mpoinfo->APP02[i].nextIFDOffset+mpoinfo->APP02[i].count_attr_IFD*12+4;
    int bytes_written=0;
    MPFLong unknown = 0xFFFFFFFF;
    j_compress_ptr cinfo=&mpoinfo->cinfo[i];
    //TODO update count_attr_IFD !
    jpeg_write_m_int16(cinfo,mpoinfo->APP02[i].count_attr_IFD);
    bytes_written+=2;
/*
    bytes_written+=mpo_write_MPExtTag(cinfo,&mpoinfo->APP02[i],MPTag_MPFVersion);

    switch (mpoinfo->APP02[0].MPentry[i].individualImgAttr.data.MPTypeCode)
    {
        //TODO: give a way to specify this value
        case MPType_MultiFrame_Panorama:
        case MPType_MultiFrame_Disparity:
        case MPType_MultiFrame_MultiAngle:
            //Unknown value
            jpeg_write_m_int16(cinfo,MPTag_IndividualNum);
            bytes_written+=2;
            bytes_written+= mpo_write_m_LONG(cinfo, &unknown, 1);
            break;
        default:
            //Not needed for the other formats, but if the format is undefined we need
            //to specify a NULL value in the case where the image is not counted in the total number of captured frames
            //Do nothing for now, since the library doesn't support this tag yet
            break;
    }

*/
    //There are no more IFD
    jpeg_write_m_int32(cinfo,0x00000000);
    bytes_written+=4;

    //hotfix, simulate attr ifd
    /* TODO : everything related to attributes */
    //TODO: finish it
    for(i=0;i<32*3;++i)
    {
        jpeg_write_m_byte(cinfo,0);
        bytes_written++;
    }

    return bytes_written;

}

long mpo_compute_MPExt_Data_size(mpo_compress_struct * mpoinfo, int image)
{
    long size=0;
    size+=4;/*MPF_identifier*/
    size+=4;/*byte_order*/
    size+=4;/*first_IFD_offset*/

    if(image==0)/*Write the Index IFD only for the first image*/
    {
        size+=2;//APP02.count
        size+=2/*tag*/ + 2 /*type*/ +4/*count*/ + 4 /*data*/; //MPTag_MPFVersion
        size+=2/*tag*/ + 2 /*type*/ +4/*count*/ + 4 /*data*/; //MPTag_NumberOfImages
        size+=2/*tag*/ + 2 /*type*/ +4/*EntriesTabLength*/ + 4 /*FirstEntryOffset*/; //MPTag_MPEntry
        size+=4;//nextIFDOffset
        size+=16*mpoinfo->APP02[0].numberOfImages;
    }
    size+=2;//mpo_write_MPExt_AttrIFD
        //Hotfix, simulates empty attr ifd....
        size+=32*3+4;//TEST


    return size;
}



void mpo_write_MPO_Marker(mpo_compress_struct * mpoinfo,int image)
{
    int length=0;
    int i;
    j_compress_ptr cinfo=&mpoinfo->cinfo[image];
    jpeg_write_m_header(cinfo,JPEG_APP0+2,mpo_compute_MPExt_Data_size(mpoinfo,image));


    for (i=0; i<4 ; ++i )
    {
        jpeg_write_m_byte(cinfo,mpoinfo->APP02[0].MPF_identifier[i]);
        length++;
    }
    jpeg_write_m_byte(cinfo,mpoinfo->APP02[0].byte_order>>24);
    length++;
    jpeg_write_m_byte(cinfo,mpoinfo->APP02[0].byte_order>>16);
    length++;
    jpeg_write_m_byte(cinfo,mpoinfo->APP02[0].byte_order>>8);
    length++;
    jpeg_write_m_byte(cinfo,mpoinfo->APP02[0].byte_order);
    length++;

    jpeg_write_m_int32(cinfo,mpoinfo->APP02[0].first_IFD_offset);
    length+=4;

    if(image==0)/*Write the Index IFD only for the first image*/
    {
        length+=mpo_write_MPExt_IndexIFD(mpoinfo);
    }

    length+=mpo_write_MPExt_AttrIFD(mpoinfo,image);



    mpo_printf("bytes wrote = %d | Computed size = %ld \n",length,mpo_compute_MPExt_Data_size(mpoinfo,image));
    assert(length==mpo_compute_MPExt_Data_size(mpoinfo,image));

}


long mpotell(mpo_compress_struct* mpoinfo,int image)
{
    return ftell(((my_dest_ptr)mpoinfo->cinfo[image].dest)->outfile)+/*Current file offset*/
                OUTPUT_BUF_SIZE-mpoinfo->cinfo[image].dest->free_in_buffer;/*Current buffer offset /!\ Wrong if buffer is flushed...*/
}


GLOBAL(void)
mpo_write_file (mpo_compress_struct* mpoinfo,char * filename)
{
    unsigned int i;

    struct jpeg_error_mgr jerr;
    /* More stuff */
    FILE * outfile;		/* target file */
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		/* physical row width in image buffer */

    long first_image_MPF_offset_begin=0;

    if ((outfile = fopen(filename, "wb")) == NULL)
    {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    for(i=0; i<mpoinfo->APP02[0].numberOfImages; ++i)
    {
        mpoinfo->cinfo[i].err = jpeg_std_error(&jerr);

        jpeg_stdio_dest(&mpoinfo->cinfo[i], outfile);

        if(i>0)
        {
            //Current offset is the absolute offset of the image to write
            //Fill the Index IFD (1st image)
            mpoinfo->APP02[0].MPentry[i].offset=ftell(((my_dest_ptr)mpoinfo->cinfo[i].dest)->outfile)-first_image_MPF_offset_begin;
            mpo_printf("Image index in 1st image Index IFD:%d\n",mpoinfo->APP02[0].MPentry[i].offset);
        }
        jpeg_set_defaults(&mpoinfo->cinfo[i]);
        mpoinfo->cinfo[i].write_JFIF_header=0;
        mpoinfo->cinfo[i].write_Adobe_marker=0;

        jpeg_start_compress(&mpoinfo->cinfo[i], TRUE);
        if(i==0)
        {
            first_image_MPF_offset_begin=mpotell(mpoinfo,i)+2+2+4;/*Begining will be 8 bytes later (starting at endianess)*/
            mpo_printf("1st image MPF offset begin at=%ld\n",first_image_MPF_offset_begin);
        }

        mpo_write_MPO_Marker(mpoinfo,i);
        mpo_printf("--------ftell=%ld\n",ftell(((my_dest_ptr)mpoinfo->cinfo[i].dest)->outfile));
        mpo_printf("--------=%d\n",OUTPUT_BUF_SIZE-mpoinfo->cinfo[i].dest->free_in_buffer);


        row_stride = mpoinfo->cinfo[i].image_width * 3;	/* JSAMPLEs per row in image_buffer */

        while (mpoinfo->cinfo[i].next_scanline < mpoinfo->cinfo[i].image_height)
        {
            row_pointer[0] = & mpoinfo->images_data[i][mpoinfo->cinfo[i].next_scanline * row_stride];
            (void) jpeg_write_scanlines(&mpoinfo->cinfo[i], row_pointer, 1);
        }
        mpo_printf("--------ftell=%ld\n",ftell(((my_dest_ptr)mpoinfo->cinfo[i].dest)->outfile));
        mpo_printf("--------=%d\n",mpoinfo->cinfo[i].dest->free_in_buffer);

        /* Step 6: Finish compression */

        jpeg_finish_compress(&mpoinfo->cinfo[i]);
        mpo_printf("---ftell=%ld\n",ftell(((my_dest_ptr)mpoinfo->cinfo[i].dest)->outfile));
        mpo_printf("--------=%d\n",mpoinfo->cinfo[i].dest->free_in_buffer);
        mpoinfo->APP02[0].MPentry[i].size= ftell(((my_dest_ptr)mpoinfo->cinfo[i].dest)->outfile)
                                        - (mpoinfo->APP02[0].MPentry[i].offset);
        if(i>0)mpoinfo->APP02[0].MPentry[i].size-=first_image_MPF_offset_begin;

        while(ftell(outfile)%16)//Align Image offset as a multiple of 16
            fputc(0,outfile);
    }

    /***************************************
    ******** Update the Index table*********
    ****************************************/
    fseek(outfile,first_image_MPF_offset_begin,SEEK_SET);
    fseek(outfile,mpoinfo->APP02[0].EntryIndex.dataOffset +4,SEEK_CUR);
    for(i=0;i<mpoinfo->APP02[0].numberOfImages;++i)
    {
        fwrite(&mpoinfo->APP02[0].MPentry[i].size,sizeof(INT32),1,outfile);
        fwrite(&mpoinfo->APP02[0].MPentry[i].offset,sizeof(INT32),1,outfile);
        fseek(outfile,8,SEEK_CUR);
    }

    fclose(outfile);

}



void mpo_init_3d_compress(mpo_compress_struct* mpoinfo,JOCTET *imageBytes_left,JOCTET *imageBytes_right,int image_width,int image_height) {
    mpo_init_compress(mpoinfo,2);
    mpo_quality_forall(mpoinfo,100);
    mpo_dimensions_forall(mpoinfo,image_width,image_height);
    //default
    mpo_colorspace_forall(mpoinfo,JCS_RGB,3);

    mpo_image_mem_src(mpoinfo,0,imageBytes_left);
    mpo_image_mem_src(mpoinfo,1,imageBytes_right);
    mpo_type_forall(mpoinfo,MPType_MultiFrame_Disparity);//This is the type for 3D images

}
