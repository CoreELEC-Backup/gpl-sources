#ifndef MPO_H_INCLUDED
#define MPO_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>


#include <stdint.h>
#include <stdbool.h>

#ifdef NDEBUG
#define mpo_printf(...)	((void)0)
#else
#define mpo_printf(...) printf(__VA_ARGS__)
#endif

/**
 * \file mpo.h
 * @brief Data structures and definitions related to mpo files
 * \author Lectem
 */

/**Those are Exif types*/
typedef enum
{
    MPF_BYTE         = 1,/*!<8-bit unsigned integer*/
    MPF_ASCII        = 2,/*!<8-bit byte containing one 7-bit ASCII code.The final byte is terminated with NULL. */
    MPF_SHORT        = 3,/*!<16-bits unsigned integer*/
    MPF_LONG         = 4,/*!<32-bits integer*/
    MPF_RATIONAL     = 5,/*!<Two LONGs. The first LONG is the numerator and the second LONG expresses the denominator. */
    MPF_UNDEFINED    = 7,/*!<Anything undefined by the EXIF standard*/
    MPF_SLONG        = 9,/*!<32-byte signed integer (2's complement notation).*/
    MPF_SRATIONAL    = 10 /*!<Two SLONGs. The first SLONG is the numerator and the second SLONG is the denominator. */
}
MPFVal_types;


typedef uint8_t	        MPFByte;          /* 1 byte  */
typedef int8_t	        MPFSByte;         /* 1 byte  */
typedef char *		    MPFAscii;
typedef uint16_t	    MPFShort;         /* 2 bytes */
typedef int16_t         MPFSShort;        /* 2 bytes */
typedef uint32_t	    MPFLong;          /* 4 bytes */
typedef int32_t		    MPFSLong;         /* 4 bytes */
typedef struct {MPFLong numerator; MPFLong denominator;} MPFRational;
typedef unsigned char	MPFUndefined;     /* 1 byte  */
typedef struct {MPFSLong numerator; MPFSLong denominator;} MPFSRational;

typedef struct
{
    MPFByte * buffer;
    long _cur;
    long _size;
}
MPFbuffer;

typedef MPFbuffer * MPFbuffer_ptr;

/**Byte order of the data
 *
 * See the DC-007_E Specification.
 * Section 5.2.2.1 Table 3, page 13
 */
typedef enum{ MPF_LITTLE_ENDIAN = 0x49492A00,
    MPF_BIG_ENDIAN = 0x4D4D002A }MPExt_ByteOrder;

/** MP format tags
 *
 * See the DC-007_E Specification.
 * Section 5.2.2.3 Table 3, page 13
 */
typedef enum
{
    /*MP Index IFD*/


    ///@{
    ///Mandatory
    MPTag_MPFVersion        = 0xB000,
    MPTag_NumberOfImages    = 0xB001,
    MPTag_MPEntry           = 0xB002,
    ///@}


    ///@{
    ///Optional
    //TODO : implement those tags
    MPTag_ImageUIDList      = 0xB003,
    MPTag_TotalFrames       = 0xB004,
    ///@}

    ///@brief Individual image tags (attributes)
    ///@{
    /**@see MPExt_ImageAttr.IndividualNum*/
    MPTag_IndividualNum     = 0xb101,
    /**@see MPExt_ImageAttr.PanOrientation*/
    MPTag_PanOrientation    = 0xb201,
    /**@see MPExt_ImageAttr.PanOverlapH*/
    MPTag_PanOverlapH       = 0xb202,
    /**@see MPExt_ImageAttr.PanOverlapV*/
    MPTag_PanOverlapV       = 0xb203,
    /**@see MPExt_ImageAttr.BaseViewpointNum*/
    MPTag_BaseViewpointNum  = 0xb204,
    /**@see MPExt_ImageAttr.ConvergenceAngle*/
    MPTag_ConvergenceAngle  = 0xb205,
    /**@see MPExt_ImageAttr.BaselineLength*/
    MPTag_BaselineLength    = 0xb206,
    /**@see MPExt_ImageAttr.VerticalDivergence*/
    MPTag_VerticalDivergence= 0xb207,
    /**@see MPExt_ImageAttr.AxisDistanceX*/
    MPTag_AxisDistanceX     = 0xb208,
    /**@see MPExt_ImageAttr.AxisDistanceY*/
    MPTag_AxisDistanceY     = 0xb209,
    /**@see MPExt_ImageAttr.AxisDistanceZ*/
    MPTag_AxisDistanceZ     = 0xb20a,
    /**@see MPExt_ImageAttr.YawAngle*/
    MPTag_YawAngle          = 0xb20b,
    /**@see MPExt_ImageAttr.PitchAngle*/
    MPTag_PitchAngle        = 0xb20c,
    /**@see MPExt_ImageAttr.RollAngle*/
    MPTag_RollAngle         = 0xb20d,

    ///@}
}MPExt_MPTags;

typedef union
{
    INT32 value;
    struct {
        unsigned int MPTypeCode:24;
        unsigned int imgType:3;
        unsigned int reserved:2;
        unsigned int representativeImage:1;
        unsigned int dependentChild:1;
        unsigned int dependentParent:1;
    } data;
}
MPExt_IndividualImageAttr;


/**
 * MP Format types
 *
 * See the DC-007_E Specification.
 * Section 5.2.3.3.1 Table 4, page 16.\n
 * Each type may have subtypes
 */
typedef enum
{
    MPType_LargeThumbnail_Mask  =0x10000, /*!<Used to check if an image is of type Large Thumbnail*/
    MPType_LargeThumbnail_Class1=0x10001, /*!<Large Thumbnail | Class 1 (VGA Equivalent)*/
    MPType_LargeThumbnail_Class2=0x10002, /*!<Large Thumbnail | Class 2 (Full HD Equivalent)*/
    MPType_MultiFrame_Mask      =0x20000, /*!<Used to check if an image is of type Multi-Frame Image*/
    MPType_MultiFrame_Panorama  =0x20001, /*!<Multi-Frame Image | Panorama*/
    MPType_MultiFrame_Disparity =0x20002, /*!<Multi-Frame Image | Disparity*/
    MPType_MultiFrame_MultiAngle=0x20003, /*!<Multi-Frame Image | Multi-Angle*/
    MPType_Baseline             =0x30000  /*!<Baseline MP Primary Image*/
}
MPExt_MPType;

typedef struct
{
    MPFShort type;
    MPFLong EntriesTabLength;
    MPFLong dataOffset;
} MPExt_MPEntryIndexFields;

typedef struct
{
    MPExt_IndividualImageAttr individualImgAttr;
    MPFLong size;
    MPFLong offset;
    MPFShort dependentImageEntry1;
    MPFShort dependentImageEntry2;
} MPExt_MPEntry;

/**MP Individual attributes
 *
 * See the DC-007_E Specification.
 * Section 5.2.4 Table 5, page 19.
 */
typedef struct
{
    /**@brief Number of the Individual Image.
    *
    * See Section 5.2.4.2 page 20. */
    MPFLong IndividualNum;

    ///@name Panorama Images attributes
    ///@{
    /**@brief Panorama Scanning Orientation
    *
    * Gives the direction, sequence and positioning of the images that comprise the final Panorama Image\n
    * See Section 5.2.4.3 page 20. */
    MPFLong PanOrientation;
    /**@brief Panorama Scanning Orientation
    *
    * Estimated amount of horizontal overlap between two adjacent images, as a percentage.\n
    * See Section 5.2.4.4 page 22. */
    MPFRational PanOverlapH;
    /**@brief Panorama Scanning Orientation
    *
    * Estimated amount of vertical overlap between two adjacent images, as a percentage.\n
    * See Section 5.2.4.4 page 22. */
    MPFRational PanOverlapV;
    ///@}

    ///@name Multi-View Images attributes
    ///@brief Used for Disparity and Multi-Angles Images
    ///@{
    MPFLong BaseViewpointNum;
    MPFSRational ConvergenceAngle;
    MPFRational BaselineLength;
    MPFSRational VerticalDivergence;
    ///@}

    ///@name Relative distance to the Target Object
    ///@{
    /**@brief Distance to the Vertical axis of the Target Object*/
    MPFSRational AxisDistanceX;
    /**@brief Distance to the Horizontal axis of the Target Object*/
    MPFSRational AxisDistanceY;
    /**@brief Distance to the Collimation axis of the Target Object*/
    MPFSRational AxisDistanceZ;
    ///@}

    /**@name Relative angle to the Target Object
     * See the Section 5.2.4.10 page 26 or the Annex A.11.Multi-Angle Image Examples page 53
     */
    ///@{
    /**@brief Vertical axis angle*/
    MPFSRational YawAngle;
    /**@brief Horizontal axis angle*/
    MPFSRational PitchAngle;
    /**@brief Collimation axis angle*/
    MPFSRational RollAngle;
    ///@}
    /**
     * Set to true if the value has to be written/was present in the file.
     * Use is_specified[TAG-MPTag_IndividualNum] to get the value
     */
    boolean is_specified[MPTag_RollAngle-MPTag_IndividualNum+1];
} MPExt_ImageAttr;

#define ATTR_IS_SPECIFIED(attr,tag) ((attr).is_specified[(tag)-MPTag_IndividualNum])


typedef struct
{
    MPFUndefined MPF_identifier[4];     /*!<Always MPF\0*/
    size_t start_of_offset;             /*!<The position of the start of this image MPO offset in the file*/
    MPExt_ByteOrder byte_order;         /*!<The endianness used in the file. The offset starts at the location of this value.*/
    MPFLong first_IFD_offset;           /*!<Offset of the first IFD of the image*/
    /*MP Index IFD*/
    INT16 count;                        /*!<Count of the MP index IFD*/
    char version[4];                    /*!<Should always be 0100*/
    MPFLong numberOfImages;             /*!<Number of Individual images*/
    MPFLong currentEntry;
    MPExt_MPEntryIndexFields EntryIndex;/*!<Number of entries and offset to the MP Entry data of each image*/
    MPFLong nextIFDOffset;              /*!<Offset of the next IFD. Used only in the first image*/

    MPFShort count_attr_IFD;            /*!<Number of Tags in the attributes IFD of the current image*/
    MPExt_ImageAttr attributes;         /*!<Individual images attributes*/

    MPExt_MPEntry* MPentry;             /*!<Available in 1st image only, details about each Individual image*/

} MPExt_Data;

typedef struct
{
    MPExt_Data* APP02;/*!<Array of each image data*/
    struct jpeg_compress_struct *cinfo;
    JOCTET ** images_data;/*!<Array pointing to each image data, used only when writing the whole MPO at once*/
} mpo_compress_struct;


bool MPExtReadMPF (MPFbuffer_ptr b,MPExt_Data *data,int isFirstImage);

char isLittleEndian();
void destroyMPF_Data(MPExt_Data *data);



#endif // MPO_H_INCLUDED
