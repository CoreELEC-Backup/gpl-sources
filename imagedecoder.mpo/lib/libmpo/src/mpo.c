#include <assert.h>
#include <stdbool.h>
#include <libmpo/mpo.h>
#include "libmpo/mpo.h"


char isLittleEndian()
{
    short int number = 0x1;
    char *numPtr = (char*)&number;
    return (numPtr[0] == 1);
}



long mpf_tell(MPFbuffer_ptr b)
{
    return b->_cur;
}

void mpf_seek(MPFbuffer_ptr b,long offset, int from)
{
    if(from==SEEK_CUR)b->_cur+=offset;
    else if(from==SEEK_SET)b->_cur=offset;
    else if(from==SEEK_END)b->_cur=b->_size-1+offset;
}

unsigned int mpf_getbyte (MPFbuffer_ptr b)
/* Read next byte */
{
    assert(b->_cur < b->_size);
    return b->buffer[b->_cur++];
}

void mpf_dc_rewindc(MPFbuffer_ptr b)
{
    b->_cur--;
}

uint16_t mpf_getint16 (MPFbuffer_ptr b, int swapEndian)
{
    if(swapEndian)
        return  mpf_getbyte(b)<<8 |
                mpf_getbyte(b);
    else
        return  mpf_getbyte(b)|
                mpf_getbyte(b)<<8;
}

uint32_t mpf_getint32 (MPFbuffer_ptr b, int swapEndian)
{
    if(swapEndian)
        return  mpf_getbyte(b)<<24|
                mpf_getbyte(b)<<16|
                mpf_getbyte(b)<<8 |
                mpf_getbyte(b);
    else
        return  mpf_getbyte(b)    |
                mpf_getbyte(b)<<8 |
                mpf_getbyte(b)<<16|
                mpf_getbyte(b)<<24;
}


int mpf_getLONG(MPFLong * value, int count,MPFbuffer_ptr b,int swapEndian)
{
    int read_bytes=0;
    read_bytes+=2;
    assert(mpf_getint16(b,swapEndian)==MPF_LONG);
    read_bytes+=4;
    assert(mpf_getint32(b,swapEndian)==(uint32_t)count);
    int i;
    for(i=0;i<count;++i)
    {
        value[i]=mpf_getint32(b,swapEndian);
        read_bytes+=4;
    }
    return read_bytes;
}


int mpf_getRATIONAL(MPFRational * value, int count,MPFbuffer_ptr b,int swapEndian)
{
    int read_bytes=0;
    read_bytes+=2;
    MPFLong type=mpf_getint16(b,swapEndian);
    assert(type==MPF_RATIONAL || type==MPF_SRATIONAL);
    read_bytes+=4;
    assert(mpf_getint32(b,swapEndian)==(uint32_t)count);
    int i;
    for(i=0;i<count;++i)
    {
        MPFLong offset=mpf_getint32(b,swapEndian);
        read_bytes+=4;
        long cur=mpf_tell(b);
        mpf_seek(b,offset,SEEK_SET);
        value[i].numerator=mpf_getint32(b,swapEndian);
        value[i].denominator=mpf_getint32(b,swapEndian);
        mpf_seek(b,cur,SEEK_SET);
    }
    return read_bytes;
}


#define mpf_getSRATIONAL(value,count,b,swapEndian) \
        mpf_getRATIONAL(((MPFRational*) value),(count),(b),(swapEndian))

void destroyMPF_Data(MPExt_Data *data)
{
    if(data->MPentry)
    {
        free(data->MPentry);
        data->MPentry=NULL;
    }
}

void print_MPFLong(MPFLong l)
{
    mpo_printf("%d",l);
}

void print_MPFRational(MPFRational r)
{
    if(r.denominator==0.0 || (r.numerator==0xFFFFFFFF&&r.denominator==0xFFFFFFFF) ) mpo_printf("Unknown");
    else mpo_printf("%f (%d/%d)",(double)r.numerator/(double)r.denominator,r.numerator,r.denominator);
}


void print_MPFSRational(MPFSRational r)
{
    if(r.denominator==0.0 || (r.numerator==(int32_t)0xFFFFFFFF&&r.denominator==(int32_t)0xFFFFFFFF) ) mpo_printf("Unknown");
    else mpo_printf("%f (%d/%d)",(double)r.numerator/(double)r.denominator,r.numerator,r.denominator);
}


boolean print_APP02_MPF (MPExt_Data *data)
{
    unsigned int i;
    if(data->MPF_identifier[0] != 'M'||
            data->MPF_identifier[1] != 'P'||
            data->MPF_identifier[2] != 'F'||
            data->MPF_identifier[3] != 0)
    {
        perror("Not an MP extended file.");
        return 0;
    }
    mpo_printf("\n\n-------------MPF extension data-------------\n");
    mpo_printf("MPF version:\t\t%.4s\n",data->version);
    if(data->byte_order == MPF_LITTLE_ENDIAN)
        mpo_printf("Byte order:\t\tlittle endian\n");
    else if(data->byte_order == MPF_BIG_ENDIAN)
        mpo_printf("Byte order:\t\tbig endian\n");
    else mpo_printf("Couldn't recognize byte order : 0x%x\n",data->byte_order);
    mpo_printf("First IFD offset:\t0x%x\n",(unsigned int)data->first_IFD_offset);
    mpo_printf("---MP Index IFD---\n");
    mpo_printf("Count:\t\t\t%d(0x%x)\n",data->count,data->count);
    if(data->numberOfImages>0) {
        mpo_printf("Number of images:\t%d\n", data->numberOfImages);
        if (data->currentEntry > 0)mpo_printf("%d entries listed\n", data->currentEntry);

        mpo_printf("----------\n");
        for (i = 0; i < data->currentEntry; ++i) {
            mpo_printf("\tSize:\t\t%d\n\tOffset:\t\t%d\n", data->MPentry[i].size, data->MPentry[i].offset);
            mpo_printf("\tDepImageEntry1:\t%d\n", data->MPentry[i].dependentImageEntry1);
            mpo_printf("\tDepImageEntry2:\t%d\n", data->MPentry[i].dependentImageEntry2);
            if (data->MPentry[i].individualImgAttr.data.imgType == 0)mpo_printf("\tData format:\tJPEG\n");
            mpo_printf("\tImage type:\t");
            switch (data->MPentry[i].individualImgAttr.data.MPTypeCode) {
                case MPType_LargeThumbnail_Class1:
                    mpo_printf("\tLarge Thumbnail (VGA)\n");
                    break;
                case MPType_LargeThumbnail_Class2:
                    mpo_printf("\tLarge Thumbnail (Full-HD)\n");
                    break;
                case MPType_MultiFrame_Panorama  :
                    mpo_printf("\tMulti-Frame Panorama\n");
                    break;
                case MPType_MultiFrame_Disparity :
                    mpo_printf("\tMulti-Frame Disparity\n");
                    break;
                case MPType_MultiFrame_MultiAngle:
                    mpo_printf("\tMulti-Frame Multi-Angle\n");
                    break;
                case MPType_Baseline             :
                    mpo_printf("\tBaseline\n");
                    break;
                default:
                    mpo_printf("\tUNDEFINED! 0x%x(value=0x%x)\n",
                               data->MPentry[i].individualImgAttr.data.MPTypeCode,
                               (unsigned int) data->MPentry[i].individualImgAttr.value);
            }
            if (data->MPentry[i].individualImgAttr.data.dependentChild)mpo_printf("\tDependent child image\n");
            if (data->MPentry[i].individualImgAttr.data.dependentParent)mpo_printf("\tDependent parent image\n");
            if (data->MPentry[i].individualImgAttr.data.representativeImage)mpo_printf("\tRepresentative image\n");

#define print_attr(TagType, fieldname, name)\
        {if(ATTR_IS_SPECIFIED(data->attributes,MPTag_ ## fieldname))\
        {\
            mpo_printf("\t" name ": ");\
            print_ ## TagType (data->attributes.fieldname);\
            mpo_printf("\n");\
        }}

            print_attr(MPFLong, IndividualNum, "MP Individual Number\t\t");
            print_attr(MPFLong, PanOrientation, "Panorama Scanning orientation\t");
            print_attr(MPFRational, PanOverlapH, "Panorama Horizontal Overlap\t");
            print_attr(MPFRational, PanOverlapV, "Panorama Vertical Overlap\t");
            print_attr(MPFLong, BaseViewpointNum, "Base Viewpoint Number\t\t");
            print_attr(MPFSRational, ConvergenceAngle, "Convergence angle\t\t");
            print_attr(MPFRational, BaselineLength, "Baseline Length\t\t\t");
            print_attr(MPFSRational, VerticalDivergence, "Vertical Divergence Angle\t");
            print_attr(MPFSRational, AxisDistanceX, "Horizontal Axis (X) distance\t");
            print_attr(MPFSRational, AxisDistanceY, "Vertical Axis (Y) distance\t");
            print_attr(MPFSRational, AxisDistanceZ, "Collimation Axis (Z) distance\t");
            print_attr(MPFSRational, YawAngle, "Yaw angle\t\t\t\t");
            print_attr(MPFSRational, PitchAngle, "Pitch angle\t\t\t\t");
            print_attr(MPFSRational, RollAngle, "Roll angle\t\t\t\t");
            mpo_printf("----------\n");
        }
    }

    mpo_printf("-----------------End of MPF-----------------\n\n\n");
    return TRUE;
}


int MPExtReadValueIFD (MPFbuffer_ptr b,MPExt_Data *data, int swapEndian)
{
    int read_bytes=0;
    data->MPentry=(MPExt_MPEntry*)calloc(data->numberOfImages,sizeof(MPExt_MPEntry));
    for(data->currentEntry=0; data->currentEntry < data->numberOfImages; data->currentEntry++)
    {
        data->MPentry[data->currentEntry].individualImgAttr.value=mpf_getint32(b,swapEndian);
        read_bytes+=4;
        data->MPentry[data->currentEntry].size=mpf_getint32(b,swapEndian);
        read_bytes+=4;
        data->MPentry[data->currentEntry].offset=mpf_getint32(b,swapEndian);
        read_bytes+=4;
        data->MPentry[data->currentEntry].dependentImageEntry1=mpf_getint16(b,swapEndian);
        read_bytes+=2;
        data->MPentry[data->currentEntry].dependentImageEntry2=mpf_getint16(b,swapEndian);
        read_bytes+=2;
    }
    return read_bytes;
}



int MPExtReadTag (MPFbuffer_ptr b,MPExt_Data *data, int swapEndian)
{
    int i;
    uint16_t tag=0;
    tag=mpf_getint16(b,swapEndian);
    int read_bytes=2;


    switch(tag)
    {
    case MPTag_MPFVersion :
        /*Specification says that the version count = 4 but that the total length of TAG+DATA is 12 */
        /*Retrieve only the 4 lasts characters, should be equal to 0100                             */
        mpf_getint16(b,swapEndian);/*Type? 07->Undefined?*/
        mpf_getint32(b,swapEndian);/*String size(Count)*/
        for(i=0; i<4; ++i)data->version[i]=mpf_getbyte(b);
        read_bytes+=10;
        break;
    case MPTag_NumberOfImages :
        /*NumberOfImages block size is 12bytes*/
        mpf_getint16(b,swapEndian);
        read_bytes+=2;/*Type ? 04-> LONG?*/
        mpf_getint32(b,swapEndian);
        read_bytes+=4;/*Count = 1 ?*/
        /*Long value = last for 4 bytes ?*/
        data->numberOfImages=mpf_getint32(b,swapEndian);
        read_bytes+=4;
        break;
    case MPTag_MPEntry:
        data->EntryIndex.type=mpf_getint16(b,swapEndian);
        read_bytes+=2;/*Type? 07 = undefined?*/
        data->EntryIndex.EntriesTabLength=mpf_getint32(b,swapEndian);
        read_bytes+=4;
        data->EntryIndex.dataOffset =mpf_getint32(b,swapEndian);
        read_bytes+=4;
        break;

    case MPTag_IndividualNum:
        read_bytes+=mpf_getLONG(&data->attributes.IndividualNum,1,b,swapEndian);
        break;
    case MPTag_PanOrientation:
        read_bytes+=mpf_getLONG(&data->attributes.PanOrientation,1,b,swapEndian);
        break;
    case MPTag_PanOverlapH:
        read_bytes+=mpf_getRATIONAL(&data->attributes.PanOverlapH,1,b,swapEndian);
        break;
    case MPTag_PanOverlapV:
        read_bytes+=mpf_getRATIONAL(&data->attributes.PanOverlapV,1,b,swapEndian);
        break;
    case MPTag_BaseViewpointNum:
        read_bytes+=mpf_getLONG(&data->attributes.BaseViewpointNum,1,b,swapEndian);
        break;
    case MPTag_ConvergenceAngle:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.ConvergenceAngle,1,b,swapEndian);
        break;
    case MPTag_BaselineLength:
        read_bytes+=mpf_getRATIONAL(&data->attributes.BaselineLength,1,b,swapEndian);
        break;
    case MPTag_VerticalDivergence:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.VerticalDivergence,1,b,swapEndian);
        break;
    case MPTag_AxisDistanceX:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.AxisDistanceX,1,b,swapEndian);
        break;
    case MPTag_AxisDistanceY:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.AxisDistanceY,1,b,swapEndian);
        break;
    case MPTag_AxisDistanceZ:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.AxisDistanceZ,1,b,swapEndian);
        break;
    case MPTag_YawAngle:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.YawAngle,1,b,swapEndian);
        break;
    case MPTag_PitchAngle:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.PitchAngle,1,b,swapEndian);
        break;
    case MPTag_RollAngle:
        read_bytes+=mpf_getSRATIONAL(&data->attributes.RollAngle,1,b,swapEndian);
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
    if(tag >=MPTag_IndividualNum && tag <= MPTag_RollAngle)
        ATTR_IS_SPECIFIED(data->attributes,tag)=1;
    return read_bytes;
}




int MPExtReadIndexIFD (MPFbuffer_ptr b,MPExt_Data *data, int swapEndian)
{
    int read_bytes=0;
    int i;
    data->count=mpf_getint16(b,swapEndian);/*Number of tags*/
    read_bytes+=2;
    for(i=0; i<data->count; ++i)
    {
        read_bytes+=MPExtReadTag(b,data,swapEndian);
    }
    data->nextIFDOffset=mpf_getint32(b,swapEndian);
    read_bytes+=4;

    read_bytes+=MPExtReadValueIFD(b,data,swapEndian);
    return read_bytes;
}


bool MPExtReadMPF (MPFbuffer_ptr b,MPExt_Data *data,int isFirstImage)
{
    int i;
    long length=b->_size;
    long OFFSET_START = length;

    data->byte_order= mpf_getint32(b,1);
    length-=4;

    int endiannessSwap=isLittleEndian() ^ (data->byte_order == MPF_LITTLE_ENDIAN);

    mpo_printf("ENDIANNESSSWAP=%d\n",endiannessSwap);
    data->first_IFD_offset=mpf_getint32(b,endiannessSwap);
    length-=4;

    while(length > (int)( OFFSET_START - data->first_IFD_offset) ) //While we didn't reach the IFD...
    {
        mpf_getbyte(b);
        length--;
    }

    if(isFirstImage)
    {
        length-=MPExtReadIndexIFD(b,data,endiannessSwap);
    }

    /*ASSUMING MP ATTRIBUTES IFD TO BE RIGHT AFTER THE VALUE OF MP INDEX IFD*/
    //TODO : use offset (nextIFD of First IFD)
    assert( (isFirstImage && (int)(OFFSET_START-data->nextIFDOffset) == length ) ||
            (int)(OFFSET_START-data->first_IFD_offset) == length);
    {
        data->count_attr_IFD=mpf_getint16(b,endiannessSwap);
        length-=2;
        for(i=0; i<data->count_attr_IFD; ++i)
        {
            //TODO: add parsing from attribute tags
            length-=MPExtReadTag(b,data,endiannessSwap);
        }
    }

    // TODO : add attrIFD
    mpo_printf("Please note that images attributes are not correct yet.\n");
    mpo_printf("bytes remaining : %ld\n",length);
    while(length-- >0)
    {
        mpo_printf("0x%.2x ",mpf_getbyte(b));
    }
    mpo_printf("\n");
    print_APP02_MPF(data);

    return 1;
}

