/* Texture functions for cs580 GzLib */
#include "StdAfx.h"
#include <stdio.h>
#include "gz.h"

GzColor	*image;
int xs = 0, ys = 0;
/* Image texture function */
int tex_fun(float u, float v, GzColor &color)
{
	static bool textmessage = false;
	static bool initiated = false;
	unsigned char icolor[3];
	float maxcolor;
	FILE *fd;

	if (!initiated)
	{
		initiated = true;

		if (0 != fopen_s(&fd, "texture.ppm", "rb"))
		{
			if (!textmessage)
			{
				textmessage = true;
				image = new GzColor[(xs + 1) * (ys + 1)];
				AfxMessageBox(_T("The texture file was not opened\n"));
			}
			return GZ_FAILURE;
		}

		fscanf_s(fd, "%*s %d %d %f%*c", &xs, &ys, &maxcolor);
		image = new GzColor[(xs + 1) * (ys + 1)];
		for (int y = 0; y < ys; y++)
		{
			for (int x = 0; x < xs; x++)
			{
				GzColor &pixel = image[y * (xs + 1) + x];
				fread(icolor, 3, 1, fd);
				pixel[0] = icolor[0] / maxcolor;
				pixel[1] = icolor[1] / maxcolor;
				pixel[2] = icolor[2] / maxcolor;
			}
		}
		fclose(fd);
	}

	u *= (xs - 1);
	v *= (ys - 1);
	if ((u < 0.f) || (u >= xs) || (v < 0.f) || (v >= ys))
	{
		return GZ_FAILURE;
	}
	int xmin = (int)u;
	int ymin = (int)v;
	float s = u - xmin;
	float t = v - ymin;
	int position = ymin * (xs + 1) + xmin;
	color[RED] = (1.f - s) * (1.f - t) * image[position][RED];
	color[GREEN] = (1.f - s) * (1.f - t) * image[position][GREEN];
	color[BLUE] = (1.f - s) * (1.f - t) * image[position][BLUE];
	position = ymin * (xs + 1) + (xmin + 1);
	color[RED] += s * (1.f - t) * image[position][RED];
	color[GREEN] += s * (1.f - t) * image[position][GREEN];
	color[BLUE] += s * (1.f - t) * image[position][BLUE];
	position = (ymin + 1) * (xs + 1) + xmin;
	color[RED] += (1.f - s) * t * image[position][RED];
	color[GREEN] += (1.f - s) * t * image[position][GREEN];
	color[BLUE] += (1.f - s) * t * image[position][BLUE];
	position = (ymin + 1) * (xs + 1) + (xmin + 1);
	color[RED] += s * t * image[position][RED];
	color[GREEN] += s * t * image[position][GREEN];
	color[BLUE] += s * t * image[position][BLUE];

	return GZ_SUCCESS;
}
/* Procedural texture function */
int ptex_fun(float u, float v, GzColor &color)
{
	float x = 5.f - 10.f * v;
	float y = 3.f - 10.f * u;
	float judge = powf(x, 4.f) + powf(y, 4.f)
		+ 100.f / (powf(x, 4.f) + powf(2.f * y - 2.f, 4.f) + powf(2.f * y - 1.f, 2.f))
		+ 100.f / (powf(x, 4.f) + powf(2.f * y + 2.f, 4.f) + powf(2.f * y + 1.f, 2.f))
		- 1.f / (powf(y + 3.f, 4.f) + powf(x / 15.f, 4.f))
		- 1.f / (powf(y + 4.f, 4.f) + powf(x / 15.f, 4.f))
		- 1.f / (powf(y + 5.f, 4.f) + powf(x / 15.f, 4.f))
		- 1.f / (4.f * powf(x + y + 4.f, 4.f) + powf((x - y + 1.f) / 5.f, 4.f))
		- powf(100.f, 16.f) / powf(powf(powf(x - 4.f, 2.f) + powf(y + 5.f, 2.f) - 13.f, 2.f)
		+ powf(powf(x - 19.f, 2.f) + powf(y + 12.f, 2.f) - 400.f, 2.f), 16.f)
		- 25.f;
	if (judge >= 0.f)
	{
		color[RED] = 1.f;
		color[GREEN] = 0.f;
		color[BLUE] = 0.f;
	}
	else
	{
		color[RED] = 0.f;
		color[GREEN] = 0.f;
		color[BLUE] = 0.f;
	}
	return GZ_SUCCESS;
}


/* Npr texture functions */
#define NPR_CATEGORY 7
int nxs = 0, nys = 0;
GzColor	*nprimage[NPR_CATEGORY];
float npr_category[NPR_CATEGORY];
inline void
npr_load_texture(void)
{
	static bool initiated = false;
	if (initiated) return;

	unsigned char icolor[3];
	float maxcolor;
	FILE *fd;
	char fname[64];
	initiated = true;
	for (int i = 0; i < NPR_CATEGORY; i++)
	{
		float gray_sum = 0.f;
		sprintf_s(fname, "objects/npr%d.ppm", i);
		if (0 != fopen_s(&fd, fname, "rb"))
		{
			AfxMessageBox(_T("Npr texture file was not opened\n"));
			exit(-1);
		}
		fscanf_s(fd, "%*s %d %d %f%*c", &nxs, &nys, &maxcolor);
		nprimage[i] = new GzColor[(nxs + 1) * (nys + 1)];
		for (int y = 0; y < nys; y++)
		{
			for (int x = 0; x < nxs; x++)
			{
				GzColor &pixel = nprimage[i][y * (nxs + 1) + x];
				fread(icolor, 3, 1, fd);
				pixel[0] = icolor[0] / maxcolor;
				pixel[1] = icolor[1] / maxcolor;
				pixel[2] = icolor[2] / maxcolor;
				gray_sum += 0.2126f * pixel[0] + 0.7152f * pixel[1] + 0.0722f * pixel[2];
			}
		}
		fclose(fd);
		gray_sum /= (float)nys * nxs;
		npr_category[i] = powf(gray_sum, 3.f) - 0.2f;
	}
}
inline int
npr_gray_category(float gray)
{
	for (int i = 0; i < NPR_CATEGORY; i++)
	{
		if (gray >= npr_category[i]) return i;
	}
	return NPR_CATEGORY - 1;
	//if (gray > 0.8f) return 0;
	//if (gray > 0.6f) return 1;
	//if (gray > 0.4f) return 2;
	//if (gray > 0.2f) return 3;
	//return 4;
}
int npr_principle(float pwx, float pwy, GzColor &color)
{
	npr_load_texture();
	while (pwx < 0.f) pwx += (nxs - 1);
	while (pwx > (nxs - 1)) pwx -= (nxs - 1);
	while (pwy < 0.f) pwy += (nys - 1);
	while (pwy > (nys - 1)) pwy -= (nys - 1);

	int xmin = (int)pwx;
	int ymin = (int)pwy;
	float s = pwx - xmin;
	float t = pwy - ymin;
	int position = ymin * (nxs + 1) + xmin;
	GzColor *nowimage = nprimage[npr_gray_category(0.2126f * color[RED] \
		+ 0.7152f * color[GREEN] + 0.0722f * color[BLUE])];
	color[RED] = (1.f - s) * (1.f - t) * nowimage[position][RED];
	color[GREEN] = (1.f - s) * (1.f - t) * nowimage[position][GREEN];
	color[BLUE] = (1.f - s) * (1.f - t) * nowimage[position][BLUE];
	position = ymin * (nxs + 1) + (xmin + 1);
	color[RED] += s * (1.f - t) * nowimage[position][RED];
	color[GREEN] += s * (1.f - t) * nowimage[position][GREEN];
	color[BLUE] += s * (1.f - t) * nowimage[position][BLUE];
	position = (ymin + 1) * (nxs + 1) + xmin;
	color[RED] += (1.f - s) * t * nowimage[position][RED];
	color[GREEN] += (1.f - s) * t * nowimage[position][GREEN];
	color[BLUE] += (1.f - s) * t * nowimage[position][BLUE];
	position = (ymin + 1) * (nxs + 1) + (xmin + 1);
	color[RED] += s * t * nowimage[position][RED];
	color[GREEN] += s * t * nowimage[position][GREEN];
	color[BLUE] += s * t * nowimage[position][BLUE];

	return GZ_SUCCESS;
}