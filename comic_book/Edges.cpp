#include    "stdafx.h"  
#include	"Gz.h"
#include    "disp.h"
#include    "Edges.h"
#define Z_MAX 2147483647

void he(int rownum, int size, int *hl, GzDisplay *display)
{
	int temp =21*16;
	int x = rownum;
	for(int y = 0; y < size-1; y++)
	{		
		int r1=display->fbuf[ARRAY(x,y)].red;
		int r2=display->fbuf[ARRAY(x,(y+1))].red;
		int g1=display->fbuf[ARRAY(x,y)].green ;
		int g2=display->fbuf[ARRAY(x,(y+1))].green;
		int b1=display->fbuf[ARRAY(x,y)].blue;
		int b2=display->fbuf[ARRAY(x,(y+1))].blue;
		if(((r1-r2)>temp)||((r1-r2)<-temp)||((g1-g2)>temp)||((g1-g2)<-temp)||((b1-b2)>temp)||((b1-b2)<-temp))
		{
			hl[ARRAY(x,y)] = 1;
			hl[ARRAY(x,(y+1))] = 1;
		}
	}
}

void ve(int colnum, int size, int *vl, GzDisplay *display)
{
	int temp = 21*16;
	int y = colnum;
	for(int x = 0; x < size-1; x++)
	{
		int r1=display->fbuf[ARRAY(x,y)].red;
		int r2=display->fbuf[ARRAY(x+1,y)].red;
		int g1=display->fbuf[ARRAY(x,y)].green ;
		int g2=display->fbuf[ARRAY(x+1,y)].green;
		int b1=display->fbuf[ARRAY(x,y)].blue;
		int b2=display->fbuf[ARRAY(x+1,y)].blue;
		if(((r1-r2)>temp)||((r1-r2)<-temp)||((g1-g2)>temp)||((g1-g2)<-temp)||((b1-b2)>temp)||((b1-b2)<-temp))
		{
			vl[ARRAY(x,y)]=1;
			vl[ARRAY((x+1),y)]=1;
		}
	
	}
}

void EdgeDec(FILE* outfile,char* framebuffer,GzDisplay* display, GzDisplay* tempDisplay)
{
	

	int *vl;
	int *hl;

	vl = new int[display->xres*display->yres];
	hl = new int[display->xres*display->yres];

	for(int i = 0; i < display->xres; i++)
	{
		for(int j= 0; j< display->yres; j++)
		{
			vl[ARRAY(i,j)] = 0;
			hl[ARRAY(i,j)] = 0;
		}
	}

	for(int i = 0; i < display->yres; i++)
	{
		ve(i, display->yres, vl, display);
		he(i, display->xres, hl, display);
	}

	//fprintf(outfile, "P6 %d %d 255\n", display->xres, display->yres);
	int count= 0;
	for(int j = 0; j < display->yres; j++)
	{
		for(int i = 0; i < display->xres; i++)
		{
			if((vl[ARRAY(i,j)] == 1)){

			display->fbuf[ARRAY(i,j)].red = 0;
			display->fbuf[ARRAY(i,j)].green = 0;
			display->fbuf[ARRAY(i,j)].blue = 0;

			unsigned char r = display->fbuf[ARRAY(i,j)].red >> 4;
			unsigned char g = display->fbuf[ARRAY(i,j)].green >> 4;
			unsigned char b = display->fbuf[ARRAY(i,j)].blue >> 4;

			framebuffer[count] = b;
			framebuffer[count+1] = g;
			framebuffer[count+2] = r;
			count=count+3;
			tempDisplay->fbuf[ARRAY(i,j)].blue = 0;
			tempDisplay->fbuf[ARRAY(i,j)].red = 0;
			tempDisplay->fbuf[ARRAY(i,j)].green = 0;
			//fprintf(outfile, "%c%c%c", r, g, b);
			}
			else if((hl[ARRAY(i,j)] == 1))
			{
			display->fbuf[ARRAY(i,j)].red = 0;
			display->fbuf[ARRAY(i,j)].green = 0;
			display->fbuf[ARRAY(i,j)].blue = 0;

			unsigned char r = display->fbuf[ARRAY(i,j)].red >> 4;
			unsigned char g = display->fbuf[ARRAY(i,j)].green >> 4;
			unsigned char b = display->fbuf[ARRAY(i,j)].blue >> 4;

			framebuffer[count] = b;
			framebuffer[count+1] = g;
			framebuffer[count+2] = r;
			count=count+3;
			tempDisplay->fbuf[ARRAY(i,j)].blue = 0;
			tempDisplay->fbuf[ARRAY(i,j)].red = 0;
			tempDisplay->fbuf[ARRAY(i,j)].green = 0;
			//fprintf(outfile, "%c%c%c", r, g, b);
			}
			else {
				tempDisplay->fbuf[ARRAY(i,j)].blue = display->fbuf[ARRAY(i,j)].blue;
				tempDisplay->fbuf[ARRAY(i,j)].red = display->fbuf[ARRAY(i,j)].red;
				tempDisplay->fbuf[ARRAY(i,j)].green = display->fbuf[ARRAY(i,j)].green;
			}
			/*else
			{




			

			
		if (display->fbuf[ARRAY(i,j)].z != Z_MAX) {
		if(display->fbuf[ARRAY(i,j)].red > 2180) { // white
			display->fbuf[ARRAY(i,j)].red = 4095; 
			display->fbuf[ARRAY(i,j)].blue = 4095; 
			display->fbuf[ARRAY(i,j)].green = 4095;
		}
		else if (display->fbuf[ARRAY(i,j)].red < 1000) { // black
			display->fbuf[ARRAY(i,j)].red = 1124; 
			display->fbuf[ARRAY(i,j)].blue = 1124; 
			display->fbuf[ARRAY(i,j)].green = 1124;
		}
		else { // grey
			display->fbuf[ARRAY(i,j)].red = 1605; 
			display->fbuf[ARRAY(i,j)].blue = 1605; 
			display->fbuf[ARRAY(i,j)].green = 1605;
		}
	}

	if (display->fbuf[ARRAY(i,j)].z != Z_MAX) {

		int noise;
		if (display->fbuf[ARRAY(i,j)].red == 1124) {
			if (rand() % 100 > 10){
				noise = display->fbuf[ARRAY(i,j)].red - rand() % 280;
			}
			else{noise = display->fbuf[ARRAY(i,j)].red;}
			display->fbuf[ARRAY(i,j)].red = noise;
			display->fbuf[ARRAY(i,j)].green = noise;
			display->fbuf[ARRAY(i,j)].blue = noise;
			
		}
		if (display->fbuf[ARRAY(i,j)].red == 1605) {
			if (rand() % 100 > 10){
				noise =display->fbuf[ARRAY(i,j)].red - rand() % 140;
			}
			else{noise = display->fbuf[ARRAY(i,j)].red;}
			display->fbuf[ARRAY(i,j)].red = noise;
		display->fbuf[ARRAY(i,j)].green = noise;
			display->fbuf[ARRAY(i,j)].blue = noise;
		}
	}
			unsigned char r = display->fbuf[ARRAY(i,j)].red >> 4;
			unsigned char g = display->fbuf[ARRAY(i,j)].green >> 4;
			unsigned char b = display->fbuf[ARRAY(i,j)].blue >> 4;

			framebuffer[count] = b;
			framebuffer[count+1] = g;
			framebuffer[count+2] = r;
			count=count+3;

			fprintf(outfile, "%c%c%c", r, g, b);
			}*/
		}
	}
}