// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    pngcmp.c

    PNG comparison (based on regrep.c)

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "osdcore.h"
#include "png.h"

#include <new>

/***************************************************************************
    CONSTANTS & DEFINES
***************************************************************************/

#define BITMAP_SPACE            4

/***************************************************************************
    PROTOTYPES
***************************************************************************/

static int generate_png_diff(const astring& imgfile1, const astring& imgfile2, const astring& outfilename);

/***************************************************************************
    MAIN
***************************************************************************/

/*-------------------------------------------------
    main - main entry point
-------------------------------------------------*/

int main(int argc, char *argv[])
{
	/* first argument is the directory */
	if (argc < 4)
	{
		fprintf(stderr, "Usage:\npngcmp <image1> <image2> <outfile>\n");
		return 10;
	}
	astring imgfilename1(argv[1]);
	astring imgfilename2(argv[2]);
	astring outfilename(argv[3]);

	try {
		return generate_png_diff(imgfilename1, imgfilename2, outfilename);
	}
	catch(...)
	{
		printf("Exception occured");
		return 1000;
	}
}

/*-------------------------------------------------
    generate_png_diff - create a new PNG file
    that shows multiple differing PNGs side by
    side with a third set of differences
-------------------------------------------------*/

static int generate_png_diff(const astring& imgfile1, const astring& imgfile2, const astring& outfilename)
{
	bitmap_argb32 bitmap1;
	bitmap_argb32 bitmap2;
	bitmap_argb32 finalbitmap;
	int width, height, maxwidth;
	core_file *file = NULL;
	file_error filerr;
	png_error pngerr;
	int error = 100;
	bool bitmaps_differ;
	int x, y;

	/* open the source image */
	filerr = core_fopen(imgfile1, OPEN_FLAG_READ, &file);
	if (filerr != FILERR_NONE)
	{
		printf("Could not open %s (%d)\n", imgfile1.cstr(), filerr);
		goto error;
	}

	/* load the source image */
	pngerr = png_read_bitmap(file, bitmap1);
	core_fclose(file);
	if (pngerr != PNGERR_NONE)
	{
		printf("Could not read %s (%d)\n", imgfile1.cstr(), pngerr);
		goto error;
	}

	/* open the source image */
	filerr = core_fopen(imgfile2, OPEN_FLAG_READ, &file);
	if (filerr != FILERR_NONE)
	{
		printf("Could not open %s (%d)\n", imgfile2.cstr(), filerr);
		goto error;
	}

	/* load the source image */
	pngerr = png_read_bitmap(file, bitmap2);
	core_fclose(file);
	if (pngerr != PNGERR_NONE)
	{
		printf("Could not read %s (%d)\n", imgfile2.cstr(), pngerr);
		goto error;
	}

	/* if the sizes are different, we differ; otherwise start off assuming we are the same */
	bitmaps_differ = (bitmap2.width() != bitmap1.width() || bitmap2.height() != bitmap1.height());

	/* compare scanline by scanline */
	for (y = 0; y < bitmap2.height() && !bitmaps_differ; y++)
	{
		UINT32 *base = &bitmap1.pix32(y);
		UINT32 *curr = &bitmap2.pix32(y);

		/* scan the scanline */
		for (x = 0; x < bitmap2.width(); x++)
			if (*base++ != *curr++)
				break;
		bitmaps_differ = (x != bitmap2.width());
	}

	if (bitmaps_differ)
	{
		/* determine the size of the final bitmap */
		height = width = 0;
		{
			/* determine the maximal width */
			maxwidth = MAX(bitmap1.width(), bitmap2.width());
			width = bitmap1.width() + BITMAP_SPACE + maxwidth + BITMAP_SPACE + maxwidth;

			/* add to the height */
			height += MAX(bitmap1.height(), bitmap2.height());
		}

		/* allocate the final bitmap */
		finalbitmap.allocate(width, height);

		/* now copy and compare each set of bitmaps */
		int curheight = MAX(bitmap1.height(), bitmap2.height());
		/* iterate over rows in these bitmaps */
		for (y = 0; y < curheight; y++)
		{
			UINT32 *src1 = (y < bitmap1.height()) ? &bitmap1.pix32(y) : NULL;
			UINT32 *src2 = (y < bitmap2.height()) ? &bitmap2.pix32(y) : NULL;
			UINT32 *dst1 = &finalbitmap.pix32(y);
			UINT32 *dst2 = &finalbitmap.pix32(y, bitmap1.width() + BITMAP_SPACE);
			UINT32 *dstdiff = &finalbitmap.pix32(y, bitmap1.width() + BITMAP_SPACE + maxwidth + BITMAP_SPACE);

			/* now iterate over columns */
			for (x = 0; x < maxwidth; x++)
			{
				int pix1 = -1, pix2 = -2;

				if (src1 != NULL && x < bitmap1.width())
					pix1 = dst1[x] = src1[x];
				if (src2 != NULL && x < bitmap2.width())
					pix2 = dst2[x] = src2[x];
				dstdiff[x] = (pix1 != pix2) ? 0xffffffff : 0xff000000;
			}
		}

		/* write the final PNG */
		filerr = core_fopen(outfilename, OPEN_FLAG_WRITE | OPEN_FLAG_CREATE, &file);
		if (filerr != FILERR_NONE)
		{
			printf("Could not open %s (%d)\n", outfilename.cstr(), filerr);
			goto error;
		}
		pngerr = png_write_bitmap(file, NULL, finalbitmap, 0, NULL);
		core_fclose(file);
		if (pngerr != PNGERR_NONE)
		{
			printf("Could not write %s (%d)\n", outfilename.cstr(), pngerr);
			goto error;
		}
	}

	/* if we get here, we are error free */
	if (bitmaps_differ)
		error = 1;
	else
		error = 0;

error:
	if (error == -1)
		osd_rmfile(outfilename);
	return error;
}
