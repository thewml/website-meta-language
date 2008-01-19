#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"

#ifdef HAVE_LIBPNG
#include <png.h>    /* includes zlib.h and setjmp.h */

#define TRUE 1
#define FALSE 0

/*---------------------------------------------------------------------------

    gd_png.c                 Copyright 1999 Greg Roelofs and Thomas Boutell

    The routines in this file, gdImagePng*() and gdImageCreateFromPng*(),
    are drop-in replacements for gdImageGif*() and gdImageCreateFromGif*(),
    except that these functions are noisier in the case of errors (comment
    out all fprintf() statements to disable that).

    Only GIF-like PNG features are currently supported; that is, images must
    either be indexed-color to begin with or they will be converted to it,
    and they can have, at most, a single, fully transparent palette entry or
    color.  (Alpha channels are ignored.)  Since gd images are artificially
    generated, gamma is also ignored, and there is currently no support for
    embedded text annotations (a la GIF comments) in gd.

    Last updated:  19 July 1999

  ---------------------------------------------------------------------------*/

typedef struct _jmpbuf_wrapper {
  jmp_buf jmpbuf;
} jmpbuf_wrapper;

static jmpbuf_wrapper gdPngJmpbufStruct;

static void gdPngErrorHandler(png_structp png_ptr, png_const_charp msg)
{
  jmpbuf_wrapper  *jmpbuf_ptr;

  /* This function, aside from the extra step of retrieving the "error
   * pointer" (below) and the fact that it exists within the application
   * rather than within libpng, is essentially identical to libpng's
   * default error handler.  The second point is critical:  since both
   * setjmp() and longjmp() are called from the same code, they are
   * guaranteed to have compatible notions of how big a jmp_buf is,
   * regardless of whether _BSD_SOURCE or anything else has (or has not)
   * been defined. */

  fprintf(stderr, "gd-png:  fatal libpng error: %s\n", msg);
  fflush(stderr);

  jmpbuf_ptr = png_get_error_ptr(png_ptr);
  if (jmpbuf_ptr == NULL) {         /* we are completely hosed now */
    fprintf(stderr,
      "gd-png:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n");
    fflush(stderr);
    exit(99);
  }

  longjmp(jmpbuf_ptr->jmpbuf, 1);
}


static void gdPngReadData(png_structp png_ptr,
	png_bytep data, png_size_t length)
{
	gdGetBuf(data, length, (gdIOCtx *)
		png_get_io_ptr(png_ptr));
}

static void gdPngWriteData(png_structp png_ptr,
	png_bytep data, png_size_t length)
{
	gdPutBuf(data, length, (gdIOCtx *)
		png_get_io_ptr(png_ptr));
}

static void gdPngFlushData(png_structp png_ptr)
{
}

void* gdImagePngPtr(gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	gdImagePngCtx(im, out);
	rv = gdDPExtractData(out, size);
	out->free(out);
	return rv;
}

/* This routine is based in part on code from Dale Lutz (Safe Software Inc.)
 *  and in part on demo code from Chapter 15 of "PNG: The Definitive Guide"
 *  (http://www.cdrom.com/pub/png/pngbook.html).
 */
void gdImagePngCtx(gdImagePtr im, gdIOCtx *outfile)
{
    int i, j, bit_depth, interlace_type;
    int width = im->sx;
    int height = im->sy;
    int colors = im->colorsTotal;
    int *open = im->open;
    int mapping[gdMaxColors];		/* mapping[gif_index] == png_index */
    png_byte trans_value = 0;
    png_color palette[gdMaxColors];
    png_structp png_ptr;
    png_infop info_ptr;
    volatile int transparent = im->transparent;
    volatile int remap = FALSE;


    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      &gdPngJmpbufStruct, gdPngErrorHandler, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate libpng main struct\n");
        return;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate libpng info struct\n");
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return;
    }

    if (setjmp(gdPngJmpbufStruct.jmpbuf)) {
        fprintf(stderr, "gd-png error: setjmp returns error condition\n");
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    png_set_write_fn(png_ptr, (void *)outfile, gdPngWriteData, gdPngFlushData);

    /* For now gd only supports palette images, for which filter type NONE is
     * almost guaranteed to be the best.  But that's what libpng defaults to
     * for palette images anyway, so no need to set this explicitly. */
/*  png_set_filter(png_ptr, 0, PNG_FILTER_NONE);  */

    /* may want to force maximum compression, but time penalty is large */
/*  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);  */

    /* can set this to a smaller value without compromising compression if all
     * image data is 16K or less; will save some decoder memory [min == 8] */
/*  png_set_compression_window_bits(png_ptr, 15);  */

    if (transparent >= im->colorsTotal ||
       (transparent >= 0 && open[transparent])) 
        transparent = -1;

    for (i = 0;  i < gdMaxColors;  ++i)
        mapping[i] = -1;

    /* count actual number of colors used (colorsTotal == high-water mark) */
    colors = 0;
    for (i = 0;  i < im->colorsTotal;  ++i) {
        if (!open[i]) {
            mapping[i] = colors;
            ++colors;
        }
    }
    if (colors < im->colorsTotal) {
        remap = TRUE;
        if (transparent >= 0)
            transparent = mapping[transparent];
    }

    if (colors <= 2)
        bit_depth = 1;
    else if (colors <= 4)
        bit_depth = 2;
    else if (colors <= 16)
        bit_depth = 4;
    else
        bit_depth = 8;

    interlace_type = im->interlace? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
      PNG_COLOR_TYPE_PALETTE, interlace_type,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    if (transparent >= 0) {
        /* always write PNG files with the transparent palette entry first to
         * minimize size of the tRNS chunk; swap if necessary */
        if (transparent != 0) {
            if (!remap) {		/* so colors == im->colorsTotal */
                remap = TRUE;
                for (i = 0;  i < colors;  ++i)
                    mapping[i] = i;
            }
            mapping[transparent] = 0;
            mapping[0] = transparent;
        }
        png_set_tRNS(png_ptr, info_ptr, &trans_value, 1, NULL);
    }

    /* convert GIF palette to libpng layout */
    if (remap)
        for (i = 0;  i < im->colorsTotal;  ++i) {
            if (mapping[i] < 0)
                continue;
            palette[mapping[i]].red   = im->red[i];
            palette[mapping[i]].green = im->green[i];
            palette[mapping[i]].blue  = im->blue[i];
        }
    else
        for (i = 0;  i < colors;  ++i) {
            palette[i].red   = im->red[i];
            palette[i].green = im->green[i];
            palette[i].blue  = im->blue[i];
        }
    png_set_PLTE(png_ptr, info_ptr, palette, colors);


    /* write out the PNG header info (everything up to first IDAT) */
    png_write_info(png_ptr, info_ptr);

    /* make sure < 8-bit images are packed into pixels as tightly as possible */
    png_set_packing(png_ptr);

    /* This code allocates a set of row buffers and copies the gd image data
     * into them only in the case that remapping is necessary; in gd 1.3 and
     * later, the im->pixels array is laid out identically to libpng's row
     * pointers and can be passed to png_write_image() function directly.
     * The remapping case could be accomplished with less memory for non-
     * interlaced images, but interlacing causes some serious complications. */
    if (remap) {
        png_bytep *row_pointers;
	row_pointers = malloc(sizeof(png_bytep) * height);
        if (row_pointers == NULL) {
            fprintf(stderr, "gd-png error: unable to allocate row_pointers\n");
        }
        for (j = 0;  j < height;  ++j) {
            if ((row_pointers[j] = (png_bytep)malloc(width)) == NULL) {
                fprintf(stderr, "gd-png error: unable to allocate rows\n");
                for (i = 0;  i < j;  ++i)
                    free(row_pointers[i]);
                return;
            }
            for (i = 0;  i < width;  ++i)
                row_pointers[j][i] = mapping[im->pixels[j][i]];
        }

        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, info_ptr);

        for (j = 0;  j < height;  ++j)
            free(row_pointers[j]);
	free(row_pointers);
    } else {
        png_write_image(png_ptr, im->pixels);
        png_write_end(png_ptr, info_ptr);
    }
    /* 1.6.3: maybe we should give that memory BACK! TBB */
        png_destroy_write_struct(&png_ptr, &info_ptr);
}

#endif /* HAVE_LIBPNG */
