#include "stdafx.h"
#include "Application5.h"
#include "Edges.h"


#define	INFILE	"ppot.asc"
#define OUTFILE "output.ppm"

extern int tex_fun(float u, float v, GzColor &color);
extern int ptex_fun(float u, float v, GzColor &color);

Application5::Application5()
{
	initiated = false;
	GzCamera camera;

	GzToken		nameListShader[9];	/* shader attribute names */
	GzPointer   valueListShader[9];	/* shader attribute pointers */
	GzToken nameListLights[10];		/* light info */
	GzPointer valueListLights[10];
	int interpStyle;
	float specpower;

	int status = 0;
	m_pUserInput = new GzInput;

	/*
	* initialize the display and renderer
	*/
	m_nWidth = 256;
	m_nHeight = 256;

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&m_pDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);

	status |= GzNewRender(&m_pRender, GZ_Z_BUFFER_RENDER, m_pDisplay);

	/* Translation matrix */
GzMatrix	scale = 
{ 
	3.25,	0.0,	0.0,	0.0, 
	0.0,	3.25,	0.0,	-3.25, 
	0.0,	0.0,	3.25,	3.5, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateX = 
{ 
	1.0,	0.0,	0.0,	0.0, 
	0.0,	.7071,	.7071,	0.0, 
	0.0,	-.7071,	.7071,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateY = 
{ 
	.866,	0.0,	-0.5,	0.0, 
	0.0,	1.0,	0.0,	0.0, 
	0.5,	0.0,	.866,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 

	/* set up app-defined camera if desired, else use camera defaults */
#if 1
	camera.position[X] = 13.2;      
  	camera.position[Y] = -8.7;
  	camera.position[Z] = -14.8;

  	camera.lookat[X] = 0.8;
  	camera.lookat[Y] = 0.7;
  	camera.lookat[Z] = 4.5;

  	camera.worldup[X] = -0.2;
  	camera.worldup[Y] = 1.0;
  	camera.worldup[Z] = 0.0;

	camera.FOV = 53.7;              /* degrees */

	status |= GzPutCamera(m_pRender, &camera);
#endif

	/* Start Renderer */
	status |= GzInitDisplay(m_pDisplay);

	status |= GzBeginRender(m_pRender);

	/* Light */
	GzLight	light1 ={ {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };// { {0.7071, 0.7071, -0.4}, {0.8, 0.8, 0.8} };//;
	GzLight	light2 =  { { 0, -0.7071, -0.7071 }, { 0.9, 0.2, 0.3 } };//{ {0.7071, 0.7071, -0.4}, {0.8, 0.8, 0.8} };
	GzLight	light3 ={ { 0.7071, 0.0, -0.7071 }, { 0.2, 0.7, 0.3 } };// { {0.7071, 0.7071, -0.4}, {0.8, 0.8, 0.8} };// 
	GzLight	ambientlight = { { 0, 0, 0 }, { 0.3, 0.3, 0.3 } };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = { 0.7, 0.7, 0.7 };

	/*
	* renderer is ready for frame --- define lights and shader at start of frame
	*/

	/*
	* Tokens associated with light parameters
	*/
	nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[0] = (GzPointer)&light1;

	nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[1] = (GzPointer)&light2;

	nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[2] = (GzPointer)&light3;

	nameListLights[3] = GZ_AMBIENT_LIGHT;
	valueListLights[3] = (GzPointer)&ambientlight;

	status |= GzPutAttribute(m_pRender, 4, nameListLights, valueListLights);

	/*
	* Tokens associated with shading
	*/
	nameListShader[0] = GZ_DIFFUSE_COEFFICIENT;
	valueListShader[0] = (GzPointer)diffuseCoefficient;

	/*
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode
	*/
	interpStyle = GZ_NORMALS;       /* Phong shading */
	nameListShader[1] = GZ_INTERPOLATE;
	valueListShader[1] = (GzPointer)&interpStyle;

	nameListShader[2] = GZ_AMBIENT_COEFFICIENT;
	valueListShader[2] = (GzPointer)ambientCoefficient;

	nameListShader[3] = GZ_SPECULAR_COEFFICIENT;
	valueListShader[3] = (GzPointer)specularCoefficient;

	specpower = 32;
	nameListShader[4] = GZ_DISTRIBUTION_COEFFICIENT;
	valueListShader[4] = (GzPointer)&specpower;

	nameListShader[5] = GZ_TEXTURE_MAP;
#if 1   /* set up null texture function or valid pointer */
	valueListShader[5] = (GzPointer)0;
#else
	valueListShader[5] = (GzPointer)(tex_fun);	/* or use ptex_fun */
#endif

	status |= GzPutAttribute(m_pRender, 6, nameListShader, valueListShader);

	status |= GzPushMatrix(m_pRender, scale);
	status |= GzPushMatrix(m_pRender, rotateY);
	status |= GzPushMatrix(m_pRender, rotateX);

	FILE *infile;
	char dummy[256];
	if (0 != fopen_s(&infile, INFILE, "r"))
	{
		AfxMessageBox(_T("The input file was not opened\n"));
		return;
	}
	ReadHelper *cache = new ReadHelper();
	while (fscanf(infile, "%s", dummy) == 1)
	{
		fscanf_s(infile, "%f %f %f %f %f %f %f %f",
			&(cache->Vertex[0][0]), &(cache->Vertex[0][1]),
			&(cache->Vertex[0][2]),
			&(cache->Normal[0][0]), &(cache->Normal[0][1]),
			&(cache->Normal[0][2]),
			&(cache->Texture[0][0]), &(cache->Texture[0][1]));
		fscanf_s(infile, "%f %f %f %f %f %f %f %f",
			&(cache->Vertex[1][0]), &(cache->Vertex[1][1]),
			&(cache->Vertex[1][2]),
			&(cache->Normal[1][0]), &(cache->Normal[1][1]),
			&(cache->Normal[1][2]),
			&(cache->Texture[1][0]), &(cache->Texture[1][1]));
		fscanf_s(infile, "%f %f %f %f %f %f %f %f",
			&(cache->Vertex[2][0]), &(cache->Vertex[2][1]),
			&(cache->Vertex[2][2]),
			&(cache->Normal[2][0]), &(cache->Normal[2][1]),
			&(cache->Normal[2][2]),
			&(cache->Texture[2][0]), &(cache->Texture[2][1]));
		mesh.push_back(*cache);
	}
	initiated = true;
	fclose(infile);
	delete cache;

	//GzPopLight(m_pRender);
	//GzPopLight(m_pRender);
	//GzPopLight(m_pRender);



	if (status) exit(GZ_FAILURE);
}

int Application5::fastRender(bool writable)
{
	GzToken nameListTriangle[3];
	GzPointer valueListTriangle[3];

	int status = 0;

	status |= GzInitDisplay(m_pDisplay);

	/*
	* Tokens associated with triangle vertex values
	*/
	nameListTriangle[0] = GZ_POSITION;
	nameListTriangle[1] = GZ_NORMAL;
	nameListTriangle[2] = GZ_TEXTURE_INDEX;

	FILE *outfile;
	if (writable)
	{
		if (0 != fopen_s(&outfile, OUTFILE, "wb"))
		{
			AfxMessageBox(_T("The output file was not opened\n"));
			return GZ_FAILURE;
		}
	}

	/*
	* Walk through the list of triangles, set color
	* and render each triangle, read in tri word
	*/
	/*GzPopLight(m_pRender);
	GzPopLight(m_pRender);
	GzPopLight(m_pRender*/
	GzLight	light1 ={ {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };// { {0.7071, 0.7071, -0.4}, {0.8, 0.8, 0.8} };//;
	GzLight	light2 =  { { 0, -0.7071, -0.7071 }, { 0.9, 0.2, 0.3 } };//{ {0.7071, 0.7071, -0.4}, {0.8, 0.8, 0.8} };
	GzLight	light3 ={ { 0.7071, 0.0, -0.7071 }, { 0.2, 0.7, 0.3 } };// { {0.7071, 0.7071, -0.4}, {0.8, 0.8, 0.8} };// 
	while (m_pRender->numlights >= 0) {
		GzPopLight(m_pRender);
	}

	GzPushLight(m_pRender, light3);
	GzPushLight(m_pRender, light2);
	GzPushLight(m_pRender, light1);
	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		valueListTriangle[0] = (GzPointer)(mesh[i].Vertex);
		valueListTriangle[1] = (GzPointer)(mesh[i].Normal);
		valueListTriangle[2] = (GzPointer)(mesh[i].Texture);

		/*
		* Set the value point//ers to the first vertex of the
		* triangle, then feed it to the renderer
		* NOTE: this sequence matches the nameList token sequence
		*/
		GzPutTriangle(m_pRender, 3, nameListTriangle, valueListTriangle);
	}
	GzDisplay* tempDisplay;
	GzNewDisplay(&tempDisplay, 1, 256, 256);// = *m_pDisplay;
	EdgeDec(outfile, m_pFrameBuffer, m_pDisplay, tempDisplay);

	if (writable)
	{
		GzFlushDisplay2File(outfile, m_pDisplay);
		fclose(outfile);
	}
	
	
	status = 0;

	status |= GzInitDisplay(m_pDisplay);

	nameListTriangle[0] = GZ_POSITION;
	nameListTriangle[1] = GZ_NORMAL;
	nameListTriangle[2] = GZ_TEXTURE_INDEX;
	
	
	while (m_pRender->numlights >= 0) {
		GzPopLight(m_pRender);
	}
	GzLight templight = {{0.7071, 0.7071, -0.4}, {0.8, 0.8, 0.8}};//;
	GzPushLight(m_pRender, templight);
	for (unsigned int i = 0; i < mesh.size(); i++) {
		valueListTriangle[0] = (GzPointer)(mesh[i].Vertex);
		valueListTriangle[1] = (GzPointer)(mesh[i].Normal);
		valueListTriangle[2] = (GzPointer)(mesh[i].Texture);
		GzPutTriangle(m_pRender, 3, nameListTriangle, valueListTriangle);
	}
	
	//EdgeDec(outfile, m_pFrameBuffer, m_pDisplay);
	//GzFlushDeepmap2FrameBuffer(m_pFrameBuffer, m_pDisplay);
	GzFlushDisplay2FrameBuffer(m_pFrameBuffer, m_pDisplay, tempDisplay);

	

	return status;
}

int Application5::Render()
{
	return fastRender(true);
}