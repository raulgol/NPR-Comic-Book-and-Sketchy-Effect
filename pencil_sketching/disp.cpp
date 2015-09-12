#include "stdafx.h"
#include "disp.h"

/*
* Control using super-sampling
*/
#define SuperSampling 2

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
	*framebuffer = new char[width * height * 3]();
	return (*framebuffer == NULL) ? GZ_FAILURE : GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes)
{
	*display = new GzDisplay();
	if (*display == NULL)
	{
		return GZ_FAILURE;
	}
	GzDisplay &play = *(*display);
	if (xRes > MAXXRES) xRes = MAXXRES;
	if (yRes > MAXYRES) yRes = MAXYRES;
#ifdef SuperSampling
	xRes *= SuperSampling;
	yRes *= SuperSampling;
#endif
	play.fbuf = new GzPixel[xRes * yRes]();
	if (play.fbuf == NULL)
	{
		delete *display;
		*display = NULL;
		return GZ_FAILURE;
	}
	play.xres = (unsigned short)xRes;
	play.yres = (unsigned short)yRes;
	play.dispClass = dispClass;
	play.open = 0;
	return GZ_SUCCESS;
}

int GzFreeDisplay(GzDisplay *display)
{
	if (display == NULL) return GZ_FAILURE;
	delete[]display->fbuf;
	delete display;
	return GZ_SUCCESS;
}

int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
	if (display == NULL) return GZ_FAILURE;
#ifdef SuperSampling
	*xRes = display->xres / SuperSampling;
	*yRes = display->yres / SuperSampling;
#else
	*xRes = display->xres;
	*yRes = display->yres;
#endif
	*dispClass = display->dispClass;
	return GZ_SUCCESS;
}

int GzInitDisplay(GzDisplay *display)
{
	if (display == NULL) return GZ_FAILURE;
	GzPixel *pflt = display->fbuf;
	for (int i = display->xres * display->yres; i > 0; i--, pflt++)
	{
		pflt->red = 4095;
		pflt->green = 4095;
		pflt->blue = 4095;
		pflt->z = 0x7FFFFFFF;
	}
	display->open = 1;
	return GZ_SUCCESS;
}

int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzDepth z)
{
	/*Rasterization Function assure boundary*/
	GzPixel *pflt = display->fbuf + ARRAY(i, j);
	pflt->red = (r < 4095) ? r : 4095;
	pflt->green = (g < 4095) ? g : 4095;
	pflt->blue = (b < 4095) ? b : 4095;
	pflt->z = z;
	return GZ_SUCCESS;
}

int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzDepth *z)
{
	if ((i < 0) || (i >= display->xres)) return GZ_FAILURE;
	if ((j < 0) || (j >= display->yres)) return GZ_FAILURE;
	GzPixel *pflt = display->fbuf + ARRAY(i, j);
	*r = pflt->red;
	*g = pflt->green;
	*b = pflt->blue;
	*z = pflt->z;
	return GZ_SUCCESS;
}

int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{
	if ((display == NULL) || (display->open == 0)) return GZ_FAILURE;
#ifdef SuperSampling
	fprintf(outfile, "P6 %d %d 255\r", display->xres / SuperSampling, display->yres / SuperSampling);
#else
	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);
#endif
	GzPixel *pflt = display->fbuf;
	unsigned int towrite = 0;
	char cache[4096];
#ifdef SuperSampling
	for (int y = display->yres / SuperSampling; y > 0; y--, pflt += (display->xres * (SuperSampling - 1)))
	{
		for (int x = display->xres / SuperSampling; x > 0; x--, pflt += SuperSampling)
		{
			unsigned int r = 0, g = 0, b = 0, xoffset = 0;
			for (int sumx = 0; sumx < SuperSampling; sumx++)
			{
				for (int sumy = 0; sumy < SuperSampling; sumy++)
				{
					r += pflt[xoffset + sumy].red;
					g += pflt[xoffset + sumy].green;
					b += pflt[xoffset + sumy].blue;
				}
				xoffset += display->xres;
			}
			cache[towrite++] = (r >> 4) / (SuperSampling * SuperSampling);
			cache[towrite++] = (g >> 4) / (SuperSampling * SuperSampling);
			cache[towrite++] = (b >> 4) / (SuperSampling * SuperSampling);
#else
	for (int y = display->yres; y > 0; y--)
	{
		for (int x = display->xres; x > 0; x--, pflt++)
		{
			cache[towrite++] = (pflt[0].red) >> 4;
			cache[towrite++] = (pflt[0].green) >> 4;
			cache[towrite++] = (pflt[0].blue) >> 4;
#endif
			if (towrite >= 4093)
			{
				fwrite(cache, towrite, 1, outfile);
				towrite = 0;
			}
		}
	}
	fwrite(cache, towrite, 1, outfile);
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{
	if ((display == NULL) || (display->open == 0)) return GZ_FAILURE;
	GzPixel *pflt = display->fbuf;
#ifdef SuperSampling
	for (int y = display->yres / SuperSampling; y > 0; y--, pflt += display->xres * (SuperSampling - 1))
	{
		for (int x = display->xres / SuperSampling; x > 0; x--, pflt += SuperSampling)
		{
			unsigned int r = 0, g = 0, b = 0, xoffset = 0;
			for (int sumx = 0; sumx < SuperSampling; sumx++)
			{
				for (int sumy = 0; sumy < SuperSampling; sumy++)
				{
					r += pflt[xoffset + sumy].red;
					g += pflt[xoffset + sumy].green;
					b += pflt[xoffset + sumy].blue;
				}
				xoffset += display->xres;
			}
			*framebuffer++ = (b >> 4) / (SuperSampling * SuperSampling);
			*framebuffer++ = (g >> 4) / (SuperSampling * SuperSampling);
			*framebuffer++ = (r >> 4) / (SuperSampling * SuperSampling);
#else
	for (int y = display->yres; y > 0; y--)
	{
		for (int x = display->xres; x > 0; x--, pflt++)
		{
			*framebuffer++ = (pflt[0].blue) >> 4;
			*framebuffer++ = (pflt[0].green) >> 4;
			*framebuffer++ = (pflt[0].red) >> 4;
#endif
		}
	}
	return GZ_SUCCESS;
}

int GzFlushDeepmap2FrameBuffer(char* framebuffer, GzDisplay* display)
{
	if ((display == NULL) || (display->open == 0)) return GZ_FAILURE;
	unsigned char *depthmap = new unsigned char[display->xres * display->yres];
	unsigned char *depthflt = depthmap;
	GzPixel *pflt = display->fbuf;
	for (int y = display->yres; y > 0; y--)
	{
		for (int x = display->xres; x > 0; x--, pflt++, depthflt++)
		{
			float dplv = pflt[0].z / (float)0x7FFFFFFF;
			register float square = dplv * dplv;
			square = square * square;
			square = square * square;
			depthflt[0] = 255 - char(255.f * square);
		}
	}
	depthflt = depthmap;
#ifdef SuperSampling
	for (int y = display->yres / SuperSampling; y > 0; y--, depthflt += display->xres * (SuperSampling - 1))
	{
		for (int x = display->xres / SuperSampling; x > 0; x--, depthflt += SuperSampling)
		{
			unsigned int gray = 0, xoffset = 0;
			for (int sumx = 0; sumx < SuperSampling; sumx++)
			{
				for (int sumy = 0; sumy < SuperSampling; sumy++)
				{
					gray += depthflt[xoffset + sumy];
				}
				xoffset += display->xres;
			}
			gray /= (SuperSampling * SuperSampling);
			*framebuffer++ = gray;
			*framebuffer++ = gray;
			*framebuffer++ = gray;
#else
	for (int y = display->yres; y > 0; y--)
	{
		for (int x = display->xres; x > 0; x--, depthflt++)
		{
			*framebuffer++ = depthflt[0];
			*framebuffer++ = depthflt[0];
			*framebuffer++ = depthflt[0];
#endif
		}
	}
	delete[]depthmap;
	return GZ_SUCCESS;
}