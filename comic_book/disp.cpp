#include "stdafx.h"
#include "disp.h"
#define Z_MAX 2147483647
/*
* Control using super-sampling
*/
//#define SuperSampling 2

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
		pflt->red = 4090;
		pflt->green = 4090;
		pflt->blue = 4090;
		pflt->alpha = 0;
		pflt->z = Z_MAX;
	}
	display->open = 1;
	return GZ_SUCCESS;
}

void trimRBG(GzIntensity* r, GzIntensity* g, GzIntensity* b) {
	if (*r < 0) {
		*r = 0;
	}
	if (*g < 0) {
		*g = 0;
	}
	if (*b < 0) {
		*b = 0;
	}
	if (*r > 4095) {
		*r = 4095;
	}
	if (*g > 4095) {
		*g = 4095;
	}
	if (*b > 4095) {
		*b = 4095;
	}
}
int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	/* write pixel values into the display */
	if (i < 0 || j < 0 || i >= display -> xres || j >= display -> yres) {
		return GZ_SUCCESS;
	}
	trimRBG(&r, &g, &b);

	
	((display -> fbuf) + ARRAY(i,j)) -> alpha = a;
	((display -> fbuf) + ARRAY(i,j)) -> blue = b;
	((display -> fbuf) + ARRAY(i,j)) -> green = g;	
	((display -> fbuf) + ARRAY(i,j)) -> red = r;
	((display -> fbuf) + ARRAY(i,j)) -> z = z;
	return GZ_SUCCESS;
}

int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	if ((i < 0) || (i >= display->xres)) return GZ_FAILURE;
	if ((j < 0) || (j >= display->yres)) return GZ_FAILURE;
	GzPixel *pflt = display->fbuf + ARRAY(i, j);
	*r = pflt->red;
	*g = pflt->green;
	*b = pflt->blue;
	*a = pflt->alpha;
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

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display, GzDisplay *tempDisplay)
{   
	int m = 0;
	for (int i = 0; i < display -> xres * display -> yres; i++) {
		if (m < display ->fbuf[i].blue) {
			m =  display ->fbuf[i].blue;
		}
		if (m < display ->fbuf[i].green) {
			m =  display ->fbuf[i].green;
		}
		if (m < display ->fbuf[i].red) {
			m =  display ->fbuf[i].red;
		}
	}
	if (m < 255) m = 255;
	m = 255;
	for(int x = 0; x < display -> xres; x++) {
		for(int y = 0; y < display -> yres; y++) {
		

			

			if (display->fbuf[ARRAY(x,y)].z != Z_MAX) {
				
		//display->fbuf[ARRAY(x,y)].red = 3400; 
			//display->fbuf[ARRAY(x,y)].blue = 3400; 
			//display->fbuf[ARRAY(x,y)].green = 3400;
		if(display->fbuf[ARRAY(x,y)].red > 2180) { // white
			display->fbuf[ARRAY(x,y)].red = 4095; 
			display->fbuf[ARRAY(x,y)].blue = 4095; 
			display->fbuf[ARRAY(x,y)].green = 4095;
		}
		else if (display->fbuf[ARRAY(x,y)].red < 1000) { // black
			display->fbuf[ARRAY(x,y)].red = 1124; 
			display->fbuf[ARRAY(x,y)].blue = 1124; 
			display->fbuf[ARRAY(x,y)].green = 1124;
		}
		else { // grey
			display->fbuf[ARRAY(x,y)].red = 1605; 
			display->fbuf[ARRAY(x,y)].blue = 1605; 
			display->fbuf[ARRAY(x,y)].green = 1605;
		}
		if(tempDisplay->fbuf[ARRAY(x,y)].red == 0 && tempDisplay->fbuf[ARRAY(x,y)].green == 0 && tempDisplay->fbuf[ARRAY(x,y)].blue == 0) { // white
					display->fbuf[ARRAY(x,y)].red = 0; 
					display->fbuf[ARRAY(x,y)].blue = 0; 
					display->fbuf[ARRAY(x,y)].green = 0;
					//continue;
				}
	}

	if (display->fbuf[ARRAY(x,y)].z != Z_MAX) {

		int noise;
		if (display->fbuf[ARRAY(x,y)].red == 1124) {
			if (rand() % 100 > 10){
				noise = display->fbuf[ARRAY(x,y)].red - rand() % 280;
			}
			else{noise = display->fbuf[ARRAY(x,y)].red;}
			display->fbuf[ARRAY(x,y)].red = noise;
			display->fbuf[ARRAY(x,y)].green = noise;
			display->fbuf[ARRAY(x,y)].blue = noise;
			
		}
		if (display->fbuf[ARRAY(x,y)].red == 1605) {
			if (rand() % 100 > 10){
				noise =display->fbuf[ARRAY(x,y)].red - rand() % 140;
			}
			else{noise = display->fbuf[ARRAY(x,y)].red;}
			display->fbuf[ARRAY(x,y)].red = noise;
		display->fbuf[ARRAY(x,y)].green = noise;
			display->fbuf[ARRAY(x,y)].blue = noise;
		}
	}
			//int rangeA = 2180;
			//int rangeB = 1000;

			/*if(((display -> fbuf) + ARRAY(x, y)) -> blue > rangeA) a = 255;
			else if (((display -> fbuf) + ARRAY(x, y)) -> blue < rangeB) a = 70;
			else a = 100;

			if(((display -> fbuf) + ARRAY(x, y)) -> green > rangeA) b = 255;
			else if (((display -> fbuf) + ARRAY(x, y)) -> green < rangeB) b = 70;
			else b = 100;

			if(((display -> fbuf) + ARRAY(x, y)) -> red > rangeA) c = 255;
			else if (((display -> fbuf) + ARRAY(x, y)) -> red < rangeB) c = 70;
			else c = 100;*/
				int a = (((display -> fbuf) + ARRAY(x, y)) -> blue);
			int b =  (((display -> fbuf) + ARRAY(x, y)) -> green);
			int c = (((display -> fbuf) + ARRAY(x, y)) -> red);
			

			a = a >> 4;
			b = b >> 4;
			c = c >> 4;
			
			*(framebuffer + 3 * ARRAY(x, y)) = a;
			*(framebuffer + 3 * ARRAY(x, y) + 1) =b;
			*(framebuffer + 3 * ARRAY(x, y) + 2) = c;

		}
	}
	return GZ_SUCCESS;
	/*
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
			*(framebuffer + 3 * ARRAY(x, y)) = (b >> 4) / (SuperSampling * SuperSampling);
			*(framebuffer + 3 * ARRAY(x, y) + 1) = (g >> 4) / (SuperSampling * SuperSampling);
			*(framebuffer + 3 * ARRAY(x, y) + 2) = (r >> 4) / (SuperSampling * SuperSampling);
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
	return GZ_SUCCESS;*/
}

int GzFlushDeepmap2FrameBuffer(char* framebuffer, GzDisplay* display)
{
	if ((display == NULL) || (display->open == 0)) return GZ_FAILURE;
	GzPixel *pflt = display->fbuf;
	GzIntensity minalpha = 4095;
	for (int y = display->yres; y > 0; y--)
	{
		for (int x = display->xres; x > 0; x--, pflt++)
		{
			/* Use alpha to store projected z value */
			pflt[0].alpha = GzIntensity(4095.f * pflt[0].z / 0x7FFFFFFF);
			if (pflt[0].alpha < minalpha) minalpha = pflt[0].alpha;
		}
	}
	pflt = display->fbuf;
	for (int y = display->yres; y > 0; y--)
	{
		for (int x = display->xres; x > 0; x--, pflt++)
		{
			pflt[0].alpha = (4095 - pflt[0].alpha) * 4095 / (4095 - minalpha);
		}
	}
	pflt = display->fbuf;
#ifdef SuperSampling
	for (int y = display->yres / SuperSampling; y > 0; y--, pflt += display->xres * (SuperSampling - 1))
	{
		for (int x = display->xres / SuperSampling; x > 0; x--, pflt += SuperSampling)
		{
			unsigned int gray = 0, xoffset = 0;
			for (int sumx = 0; sumx < SuperSampling; sumx++)
			{
				for (int sumy = 0; sumy < SuperSampling; sumy++)
				{
					gray += pflt[xoffset + sumy].alpha;
				}
				xoffset += display->xres;
			}
			*framebuffer++ = (gray >> 4) / (SuperSampling * SuperSampling);
			*framebuffer++ = (gray >> 4) / (SuperSampling * SuperSampling);
			*framebuffer++ = (gray >> 4) / (SuperSampling * SuperSampling);
#else
	for (int y = display->yres; y > 0; y--)
	{
		for (int x = display->xres; x > 0; x--, pflt++)
		{
			*framebuffer++ = (pflt[0].alpha) >> 4;
			*framebuffer++ = (pflt[0].alpha) >> 4;
			*framebuffer++ = (pflt[0].alpha) >> 4;
#endif
		}
	}
	return GZ_SUCCESS;
}