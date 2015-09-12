#ifndef GZ_DISPLAY
#define GZ_DISPLAY

#include <stdio.h>
#include "gz.h"

/*
* define general display pixel-type
*/
typedef	struct GzPixel{
	GzIntensity    red;
	GzIntensity    green;
	GzIntensity    blue;
	GzIntensity    alpha;
	GzDepth	 z;
} GzPixel;

/*
*define a display type
*/
struct GzDisplay
{
	unsigned short	xres;
	unsigned short	yres;
	GzDisplayClass	dispClass;
	short			open;
	GzPixel         *fbuf;
};

#define	MAXXRES	1024	/* put some bounds on size in case of error */
#define	MAXYRES	1024

#define	ARRAY(x,y)	(x+(y*display->xres))	/* simplify fbuf indexing */

// Function declaration
int GzNewFrameBuffer(char** framebuffer, int width, int height);
int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes);
int GzFreeDisplay(GzDisplay *display);
int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass);
int GzInitDisplay(GzDisplay *display);
int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzDepth z);
int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzDepth *z);
int GzFlushDisplay2File(FILE* outfile, GzDisplay *display);
int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay* display);
int GzFlushDeepmap2FrameBuffer(char* framebuffer, GzDisplay* display);

#endif  /*GZ_DISPLAY included*/