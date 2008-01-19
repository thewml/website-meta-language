#ifdef PERL_CAPI
#define WIN32IO_IS_STDIO
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "libgd/gd.h"
#ifdef FCGI
 #include <fcgi_stdio.h>
#else
 #ifdef USE_SFIO
  #include <config.h>
 #else
  #include <stdio.h>
 #endif
 #include <perlio.h>
#endif
/* Copyright 1995 - 1998, Lincoln D. Stein.  See accompanying README file for
	usage restrictions */

typedef gdImagePtr	WML__GD__Image;
typedef PerlIO          * InputStream;

MODULE = WML::GD		PACKAGE = WML::GD::Image	PREFIX=gd

WML::GD::Image
gdnew(packname="WML::GD::Image", x=64, y=64)
	char *	packname
	int	x
	int	y
        PROTOTYPE: $;$$
	CODE:
	{
		gdImagePtr theImage;
		theImage = gdImageCreate(x,y);
		RETVAL = theImage;
	}
	OUTPUT:
		RETVAL

SV*
gdpng(image)
  WML::GD::Image	image
  PROTOTYPE: $
  CODE:
  {
	int           size;
#ifdef HAVE_LIBPNG
	void*         data;
	data = (void *) gdImagePngPtr(image,&size);
#else
	const char*   data = '\0';
        size = 1;
croak("libgd was compiled without support of the PNG image format");
#endif
	RETVAL = newSVpv((char*) data,size);
#ifdef HAVE_LIBPNG
	free(data);
#endif
  }
  OUTPUT:
    RETVAL

SV*
gdgif(image)
  WML::GD::Image	image
  PROTOTYPE: $
  CODE:
  {
	void*         data;
	int           size;
 	data = (void *) gdImageGifPtr(image,&size);
	RETVAL = newSVpv((char*) data,size);
	free(data);
  }
  OUTPUT:
    RETVAL

int
gdtransparent(image, ...)
	WML::GD::Image	image
        PROTOTYPE: $;$
	CODE:
	{
		int color;
		if (items > 1) {
			color=(int)SvIV(ST(1));
			gdImageColorTransparent(image,color);
		}
		RETVAL = gdImageGetTransparent(image);
	}
	OUTPUT:
		RETVAL

void
gdline(image,x1,y1,x2,y2,color)
	WML::GD::Image	image
	int		x1
	int		y1
	int		x2
	int		y2
	int		color
        PROTOTYPE: $$$$$$
	CODE:
	{
		gdImageLine(image,x1,y1,x2,y2,color);
	}

void
gdfilledRectangle(image,x1,y1,x2,y2,color)
	WML::GD::Image	image
	int		x1
	int		y1
	int		x2
	int		y2
	int		color
        PROTOTYPE: $$$$$$
	CODE:
	{
		gdImageFilledRectangle(image,x1,y1,x2,y2,color);
	}

int
colorAllocate(image,r,g,b)
	WML::GD::Image	image
	int		r
	int		g
	int		b
        PROTOTYPE: $$$$
	CODE:
	{
		RETVAL = gdImageColorAllocate(image,r,g,b);
	}
	OUTPUT:
		RETVAL
