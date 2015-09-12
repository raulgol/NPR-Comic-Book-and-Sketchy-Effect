#include "stdafx.h"
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <opencv2\opencv.hpp>
#include "Application5.h"
using namespace std;

#define OUTFILE "output.ppm"

extern int tex_fun(float u, float v, GzColor &color);
extern int ptex_fun(float u, float v, GzColor &color);
extern int npr_principle(float pwx, float pwy, GzColor &color);

static inline void
Tokenize(const string& str, vector<string>& tokens)
{
	string::size_type lastPos = str.find_first_not_of(' ', 0);
	string::size_type pos = str.find_first_of(' ', lastPos);

	while ((string::npos != pos) || (string::npos != lastPos))
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(' ', pos);
		pos = str.find_first_of(' ', lastPos);
	}
}
static inline void
safeGetline(istream& is, string& t)
{
	t.clear();
	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	while (true)
	{
		int c = sb->sbumpc();
		switch (c)
		{
		case '\n':
			return;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return;
		case EOF:
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return;
		default:
			t += (char)c;
		}
	}
}

int Application5::operator+=(ObjHelper &src)
{
	unsigned int i;
	for (i = 0; i < mesh.size(); i++)
	{
		if ((mesh[i].Vertex[0] == src.Vertex[0]) &&
			(mesh[i].Vertex[1] == src.Vertex[1]) &&
			(mesh[i].Vertex[2] == src.Vertex[2]) &&
			(mesh[i].Texture[0] == src.Texture[0]) &&
			(mesh[i].Texture[1] == src.Texture[1])) break;
	}
	if (i != mesh.size())
	{
		mesh[i].Normal[0] = mesh[i].Normal[0] * mesh[i].totalnorm + src.Normal[0];
		mesh[i].Normal[1] = mesh[i].Normal[1] * mesh[i].totalnorm + src.Normal[1];
		mesh[i].Normal[2] = mesh[i].Normal[2] * mesh[i].totalnorm + src.Normal[2];
		mesh[i].totalnorm++;
		mesh[i].Normal[0] /= mesh[i].totalnorm;
		mesh[i].Normal[1] /= mesh[i].totalnorm;
		mesh[i].Normal[2] /= mesh[i].totalnorm;
	}
	else
	{
		mesh.push_back(src);
	}
	return (int)i;
}
void Application5::MeshFileInput(wstring fname)
{
	string sLine;
	vector<string> cache;
	ifstream infile(fname);
	if (!infile.is_open())
	{
		AfxMessageBox(_T("Can't open file!\n"));
		return;
	}

	ObjHelper toread;
	int directinput = 3;
	int directinputpos[3];
	vector<vector<float>> vertexlist;
	vector<vector<float>> normallist;
	vector<vector<float>> texturelist;

	mesh.clear();
	facelist.clear();
	while (!infile.eof())
	{
		cache.clear();
		safeGetline(infile, sLine);
		if (sLine == "") continue;
		if (directinput != 3)
		{
			sscanf_s(sLine.data(), "%f %f %f %f %f %f %f %f",
				&(toread.Vertex[0]), &(toread.Vertex[1]), &(toread.Vertex[2]),
				&(toread.Normal[0]), &(toread.Normal[1]), &(toread.Normal[2]),
				&(toread.Texture[0]), &(toread.Texture[1]));
			directinputpos[directinput++] = (*this += toread);
			if (directinput == 3)
			{
				vector<int> fc(3);
				fc[0] = directinputpos[0];
				fc[1] = directinputpos[1];
				fc[2] = directinputpos[2];
				facelist.push_back(fc);
			}
			continue;
		}
		Tokenize(sLine, cache);
		if (cache[0].substr(0, 8) == "triangle")
		{
			directinput = 0;
			continue;
		}
		if (cache[0] == "v")
		{
			float XYZ[3] = {
				stof(cache[1]), stof(cache[2]), stof(cache[3])
			};
			vector<float> temp(3);
			temp[0] = XYZ[0];
			temp[1] = XYZ[1];
			temp[2] = XYZ[2];
			vertexlist.push_back(temp);
			continue;
		}
		if (cache[0] == "vn")
		{
			float XYZ[3] = {
				stof(cache[1]), stof(cache[2]), stof(cache[3])
			};
			vector<float> temp(3);
			temp[0] = XYZ[0];
			temp[1] = XYZ[1];
			temp[2] = XYZ[2];
			normallist.push_back(temp);
			continue;
		}
		if (cache[0] == "vt")
		{
			float UV[2] = {
				stof(cache[1]), stof(cache[2])
			};
			vector<float> temp(2);
			temp[0] = UV[0];
			temp[1] = UV[1];
			texturelist.push_back(temp);
			continue;
		}
		if (cache[0] == "f")
		{
			int record[32][3];
			unsigned int index = 0;
			for (unsigned int i = 1; i < cache.size(); i++)
			{
				if (sscanf_s(cache[i].data(), "%d/%d/%d", &record[index][0], &record[index][1], &record[index][2]) != 3)
				{
					if (sscanf_s(cache[i].data(), "%d//%d", &record[index][0], &record[index][2]) != 2)
					{
						sscanf_s(cache[i].data(), "%d", &record[index][0]);
						record[index][2] = 0;
					}
					record[index][1] = 0;
				}
				index++;
			}
			for (unsigned int loop = 0; loop < index; loop++)
			{
				int vindex = record[loop][0] - 1;
				int tindex = record[loop][1] - 1;
				int nindex = record[loop][2] - 1;
				toread.Vertex[0] = vertexlist[vindex][0];
				toread.Vertex[1] = vertexlist[vindex][1];
				toread.Vertex[2] = vertexlist[vindex][2];
				if (tindex >= 0)
				{
					toread.Texture[0] = texturelist[tindex][0];
					toread.Texture[1] = texturelist[tindex][1];
				}
				if (nindex >= 0)
				{
					toread.Normal[0] = normallist[nindex][0];
					toread.Normal[1] = normallist[nindex][1];
					toread.Normal[2] = normallist[nindex][2];
				}
				record[loop][0] = (*this += toread);
			}
			vector<int> ftemp(3);
			ftemp[0] = record[0][0];
			for (unsigned int i = 2; i < index; i++)
			{
				ftemp[1] = record[i - 1][0];
				ftemp[2] = record[i][0];
				facelist.push_back(ftemp);
			}
			continue;
		}
	}
	/* Calculate Principle Curves */
	MeshCalculation(false);
}

static inline float
NormalDotProduct(const GzCoord &A, const GzCoord &B)
{
	return (A[X] * B[X] + A[Y] * B[Y] + A[Z] * B[Z]);
}
static inline void
NormalDecoupling(GzCoord &This, const GzCoord &ref)
{
	float coef = NormalDotProduct(This, ref);
	This[X] -= ref[X] * coef;
	This[Y] -= ref[Y] * coef;
	This[Z] -= ref[Z] * coef;
}
static inline void
NormalNormal(GzCoord &norm)
{
	float len = sqrtf(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
	if (len != 0.f)
	{
		norm[0] /= len;
		norm[1] /= len;
		norm[2] /= len;
	}
}
void Application5::MeshCalculation(bool subdivision)
{
	if (subdivision)
	{
		GzCoord Vertex[3];
		GzCoord Normal[3];
		GzTextureIndex Texture[3];
		for (int f = facelist.size() - 1; f >= 0; f--)
		{
			for (int loop = 0; loop < 3; loop++)
			{
				ObjHelper &nmesh = mesh[facelist[f][loop]];
				Vertex[loop][0] = nmesh.Vertex[0];
				Vertex[loop][1] = nmesh.Vertex[1];
				Vertex[loop][2] = nmesh.Vertex[2];
				Normal[loop][0] = nmesh.Normal[0];
				Normal[loop][1] = nmesh.Normal[1];
				Normal[loop][2] = nmesh.Normal[2];
				Texture[loop][0] = nmesh.Texture[0];
				Texture[loop][1] = nmesh.Texture[1];
			}
#ifndef CoordAdd
#define CoordAdd(c, a, b) {c[b][X] + c[a][X], c[b][Y] + c[a][Y], c[b][Z] + c[a][Z]}
#define CoordSub(c, a, b) {c[b][X] - c[a][X], c[b][Y] - c[a][Y], c[b][Z] - c[a][Z]}
#define CoordLen(This) sqrtf(This[X] * This[X] + This[Y] * This[Y] + This[Z] * This[Z])
#define CoordCopy(This, src) {This[X] = src[X]; This[Y] = src[Y]; This[Z] = src[Z];}
#define TextureAvg(c, a, b) {(c[b][U] + c[a][U]) * 0.5f, (c[b][V] + c[a][V]) * 0.5f}
#endif
			/* mesh Subdivision start */
			GzCoord Pabc[3] = {
				CoordSub(Vertex, 1, 2),
				CoordSub(Vertex, 2, 0),
				CoordSub(Vertex, 0, 1)
			};
			const float Pdist[3] = {
				CoordLen(Pabc[0]), CoordLen(Pabc[1]), CoordLen(Pabc[2])
			};
			/* cubic Bezier curve (Hermit data) */
			GzCoord Tangents[6] = {
				CoordSub(Vertex, 1, 2), CoordSub(Vertex, 2, 0),
				CoordSub(Vertex, 0, 1), CoordSub(Vertex, 2, 1),
				CoordSub(Vertex, 0, 2), CoordSub(Vertex, 1, 0)
			};
			NormalDecoupling(Tangents[0], Normal[1]);
			NormalDecoupling(Tangents[1], Normal[2]);
			NormalDecoupling(Tangents[2], Normal[0]);
			NormalDecoupling(Tangents[3], Normal[2]);
			NormalDecoupling(Tangents[4], Normal[0]);
			NormalDecoupling(Tangents[5], Normal[1]);
			for (int i = 0; i < 6; i++)
			{
				NormalNormal(Tangents[i]);
			}
			GzCoord SubVertex[3] = {
				CoordAdd(Vertex, 1, 2),
				CoordAdd(Vertex, 2, 0),
				CoordAdd(Vertex, 0, 1)
			};
			for (int i = 0; i < 3; i++)
			{
				GzCoord Cache = CoordAdd(Tangents, i, i + 3);
				SubVertex[i][X] = SubVertex[i][X] * 0.5f + Cache[X] * Pdist[i] * 0.125f;
				SubVertex[i][Y] = SubVertex[i][Y] * 0.5f + Cache[Y] * Pdist[i] * 0.125f;
				SubVertex[i][Z] = SubVertex[i][Z] * 0.5f + Cache[Z] * Pdist[i] * 0.125f;
			}
			GzCoord SubNormal[3] = {
				CoordAdd(Normal, 1, 2),
				CoordAdd(Normal, 2, 0),
				CoordAdd(Normal, 0, 1)
			};
			for (int i = 0; i < 3; i++)
			{
				GzCoord Cache = CoordSub(Tangents, i, i + 3);
				Pabc[i][X] += Cache[X] * Pdist[i] / 6.f;
				Pabc[i][Y] += Cache[Y] * Pdist[i] / 6.f;
				Pabc[i][Z] += Cache[Z] * Pdist[i] / 6.f;
				NormalNormal(Pabc[i]);
				NormalDecoupling(SubNormal[i], Pabc[i]);
				NormalNormal(SubNormal[i]);
			}
			GzTextureIndex SubTexture[3] = {
				TextureAvg(Texture, 1, 2),
				TextureAvg(Texture, 2, 0),
				TextureAvg(Texture, 0, 1)
			};
			/* Finished compute */
			ObjHelper toadd;
			int newVertexIndex[3];
			for (int loop = 0; loop < 3; loop++)
			{
				CoordCopy(toadd.Vertex, SubVertex[loop]);
				CoordCopy(toadd.Normal, SubNormal[loop]);
				toadd.Texture[U] = SubTexture[loop][U];
				toadd.Texture[V] = SubTexture[loop][V];
				newVertexIndex[loop] = (*this += toadd);
			}
			vector<int> facenew(3);
			facenew[0] = facelist[f][0];
			facenew[1] = newVertexIndex[1];
			facenew[2] = newVertexIndex[2];
			facelist.push_back(facenew);
			facenew[0] = facelist[f][1];
			facenew[1] = newVertexIndex[2];
			facenew[2] = newVertexIndex[0];
			facelist.push_back(facenew);
			facenew[0] = facelist[f][2];
			facenew[1] = newVertexIndex[0];
			facenew[2] = newVertexIndex[1];
			facelist.push_back(facenew);
			facelist[f][0] = newVertexIndex[0];
			facelist[f][1] = newVertexIndex[1];
			facelist[f][2] = newVertexIndex[2];
		}
	}

	/* Compute principle curvature */
	principalCurvature();
}

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
	m_nWidth = 512;
	m_nHeight = 512;

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&m_pDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);

	status |= GzNewRender(&m_pRender, GZ_Z_BUFFER_RENDER, m_pDisplay);

	/* Translation matrix */
	GzMatrix	scale =
	{
		3.25, 0.0, 0.0, 0.0,
		0.0, 3.25, 0.0, -3.25,
		0.0, 0.0, 3.25, 3.5,
		0.0, 0.0, 0.0, 1.0
	};
	GzMatrix	rotateX =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, .7071, .7071, 0.0,
		0.0, -.7071, .7071, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	GzMatrix	rotateY =
	{
		.866, 0.0, -0.5, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.5, 0.0, .866, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	/* set up app-defined camera if desired, else use camera defaults */
#if 1
	camera.position[X] = -3;
	camera.position[Y] = -25;
	camera.position[Z] = -4;

	camera.lookat[X] = 7.8;
	camera.lookat[Y] = 0.7;
	camera.lookat[Z] = 6.5;

	camera.worldup[X] = -0.2;
	camera.worldup[Y] = 1.0;
	camera.worldup[Z] = 0.0;

	camera.FOV = 63.7;              /* degrees */

	status |= GzPutCamera(m_pRender, &camera);
#endif

	/* Start Renderer */
	status |= GzInitDisplay(m_pDisplay);

	status |= GzBeginRender(m_pRender);

	/* Light */
	GzLight	light1 = { { 0.7071, 0.7071, 0 }, { 1.0, 1.0, 1.0 } };
	//GzLight	light1 = { { -0.7071, 0.7071, 0 }, { 0.5, 0.5, 0.9 } };
	GzLight	light2 = { { 0, -0.7071, -0.7071 }, { 0.9, 0.2, 0.3 } };
	GzLight	light3 = { { 0.7071, 0.0, -0.7071 }, { 0.2, 0.7, 0.3 } };
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
	valueListShader[5] = (GzPointer)NULL;
	/* or use ptex_fun */
#if 1
	m_pRender->tex_bak = (tex_fun);
	m_pRender->npr_fun = (npr_principle);
#endif

	status |= GzPutAttribute(m_pRender, 6, nameListShader, valueListShader);

	status |= GzPushMatrix(m_pRender, scale);
	status |= GzPushMatrix(m_pRender, rotateY);
	status |= GzPushMatrix(m_pRender, rotateX);

	initiated = true;

	if (status) exit(GZ_FAILURE);
}

int Application5::fastRender(bool writable)
{
	GzToken nameListTriangle[5];
	GzPointer valueListTriangle[5];

	int status = 0;

	status |= GzInitDisplay(m_pDisplay);

	/*
	* Tokens associated with triangle vertex values
	*/
	nameListTriangle[0] = GZ_POSITION;
	nameListTriangle[1] = GZ_NORMAL;
	nameListTriangle[2] = GZ_TEXTURE_INDEX;
	nameListTriangle[3] = GZ_MINCURVATURE;
	nameListTriangle[4] = GZ_MAXCURVATURE;

	FILE *outfile;
	if (writable)
	{
		if (0 != fopen_s(&outfile, OUTFILE, "wb"))
		{
			AfxMessageBox(_T("The output file was not opened\n"));
			return GZ_FAILURE;
		}
	}

	GzCoord Nvert[3];
	GzCoord Nnorm[3];
	GzTextureIndex Ntext[3];
	GzCoord Nmincurve[3];
	GzCoord Nmaxcurve[3];
	valueListTriangle[0] = (GzPointer)&Nvert;
	valueListTriangle[1] = (GzPointer)&Nnorm;
	valueListTriangle[2] = (GzPointer)&Ntext;
	valueListTriangle[3] = (GzPointer)&Nmincurve;
	valueListTriangle[4] = (GzPointer)&Nmaxcurve;

	for (unsigned int i = 0; i < facelist.size(); i++)
	{
		//fprintf_s(outfile, "triangle\n");
		for (int loop = 0; loop < 3; loop++)
		{
			ObjHelper &nmesh = mesh[facelist[i][loop]];
			Nvert[loop][0] = nmesh.Vertex[0];
			Nvert[loop][1] = nmesh.Vertex[1];
			Nvert[loop][2] = nmesh.Vertex[2];
			Nnorm[loop][0] = nmesh.Normal[0];
			Nnorm[loop][1] = nmesh.Normal[1];
			Nnorm[loop][2] = nmesh.Normal[2];
			Ntext[loop][0] = nmesh.Texture[0];
			Ntext[loop][1] = nmesh.Texture[1];
			Nmincurve[loop][0] = nmesh.MinCurve[0];
			Nmincurve[loop][1] = nmesh.MinCurve[1];
			Nmincurve[loop][2] = nmesh.MinCurve[2];
			Nmaxcurve[loop][0] = nmesh.MaxCurve[0];
			Nmaxcurve[loop][1] = nmesh.MaxCurve[1];
			Nmaxcurve[loop][2] = nmesh.MaxCurve[2];
			//fprintf_s(outfile, "%f %f %f %f %f %f %f %f\n", Nvert[loop][0], Nvert[loop][1], Nvert[loop][2], Nnorm[loop][0], Nnorm[loop][1], Nnorm[loop][2], Ntext[loop][0], Ntext[loop][1]);
		}

		GzPutTriangle(m_pRender, 5, nameListTriangle, valueListTriangle);
	}

	//GzFlushDeepmap2FrameBuffer(m_pFrameBuffer, m_pDisplay);
	GzFlushDisplay2FrameBuffer(m_pFrameBuffer, m_pDisplay);

	if (writable)
	{
		GzFlushDisplay2File(outfile, m_pDisplay);
		fclose(outfile);
	}

	return status;
}

int Application5::Render()
{
	return fastRender(true);
}

void Application5::vertex_neighbours(vector<vector<int>> &neighborList)
{
	int numV = mesh.size();
	int numF = facelist.size();

	//loop through all faces
	for (int i = 0; i < numF; i++)
	{
		int a = facelist[i][0];
		int b = facelist[i][1];
		int c = facelist[i][2];

		//put vertex 1 and 2 to the neighbor list of vertex 0
		neighborList[a].push_back(b);
		neighborList[a].push_back(c);
		//put vertex 0 and 2 to the neighbor list of vertex 1
		neighborList[b].push_back(a);
		neighborList[b].push_back(c);
		//put vertex 0 and 1 to the neighbor list of vertex 2
		neighborList[c].push_back(a);
		neighborList[c].push_back(b);
	}
	//loop through all neighbor arrays and sort then (rotation same as faces)
	for (int i = 0; i < numV; i++)
	{
		vector<int> Pneighf = neighborList[i];
		int start = 0;
		bool found = false;
		for (int index1 = 0; index1 < Pneighf.size(); index1 += 2)
		{
			found = false;
			for (int index2 = 1; index2 < Pneighf.size(); index2 += 2)
			{
				if (Pneighf[index1] == Pneighf[index2])
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				start = index1;
				break;
			}
		}
		vector<int> Pneig(2);
		Pneig[0] = Pneighf[start];
		Pneig[1] = Pneighf[start + 1];
		//add the neighbors with respect to original rotation
		for (int j = 1 + (int)(found); j < Pneighf.size() / 2; j++)
		{
			found = false;
			for (int index1 = 0; index1 < Pneighf.size(); index1 += 2)
			{
				if (Pneighf[index1] == Pneighf[Pneighf.size() - 1])
				{
					bool exist = false;
					for (int pp = 0; pp < Pneig.size(); pp++)
					{
						if (Pneig[pp] == Pneighf[index1 + 1])
						{
							exist = true;
							break;
						}
					}
					if (!exist)
					{
						found = true;
						Pneig.push_back(Pneighf[index1 + 1]);
					}
				}
			}
			if (!found)
			{
				for (int index = 0; index < Pneighf.size(); index += 2)
				{
					bool exist = false;
					for (int pp = 0; pp < Pneig.size(); pp++)
					{
						if (Pneig[pp] == Pneighf[index])
						{
							exist = true;
							break;
						}
					}
					if (!exist)
					{
						Pneig.push_back(Pneighf[index]);
						bool exist2 = false;
						for (int pp = 0; pp < Pneig.size(); pp++)
						{
							if (Pneig[pp] == Pneighf[index + 1])
							{
								exist2 = true;
								break;
							}
						}
						if (!exist2)
						{
							Pneig.push_back(Pneighf[index + 1]);
						}
					}
				}
			}

		}
		//add forgotten neighbors
		if (Pneig.size() < Pneighf.size())
		{
			for (int j = 0; j < Pneighf.size(); j++)
			{
				bool exist = false;
				for (int pp = 0; pp < Pneig.size(); pp++)
				{
					if (Pneig[pp] == Pneighf[j])
					{
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					Pneig.push_back(Pneighf[j]);
				}
			}
		}
		neighborList[i] = Pneig;
	}
}
static inline void
VectorRotationMatrix(GzCoord &norm, vector<vector<float>> &M, vector<vector<float>> &Minv)
{
	NormalNormal(norm);
	srand((unsigned)time(0));
	GzCoord l, k;
	for (int i = 0; i < 3; i++)
	{
		k[i] = rand() / (float)(RAND_MAX);
	}
	l[0] = k[1] * norm[2] - k[2] * norm[1];
	l[1] = k[2] * norm[0] - k[0] * norm[2];
	l[2] = k[0] * norm[1] - k[1] * norm[0];
	NormalNormal(l);

	k[0] = l[1] * norm[2] - l[2] * norm[1];
	k[1] = l[2] * norm[0] - l[0] * norm[2];
	k[2] = l[0] * norm[1] - l[1] * norm[0];
	NormalNormal(k);

	for (int i = 0; i < 3; i++)
	{
		Minv[i][0] = norm[i];
		Minv[i][1] = l[i];
		Minv[i][2] = k[i];
	}
	CvMat *matrix = cvCreateMat(3, 3, CV_32FC1);
	CvMat *inverse = cvCreateMat(3, 3, CV_32FC1);
	for (int i = 0; i < matrix->rows; i++) {
		for (int j = 0; j < matrix->cols; j++) {
			cvmSet(matrix, i, j, Minv[i][j]);
		}
	}
	cvInvert(matrix, inverse, CV_SVD);
	for (int i = 0; i < matrix->rows; i++) {
		for (int j = 0; j < matrix->cols; j++) {
			M[i][j] = cvmGet(inverse, i, j);
		}
	}
}
static inline void
eig2(vector<float>& I1, vector<float>& I2, float xx, float xy, float yy)
{
	float tmp = sqrt((xx - yy)*(xx - yy) + 4 * xy*xy);
	float v2x = 2 * xy;
	float v2y = yy - xx + tmp;
	float mag = sqrt(v2x*v2x + v2y*v2y);
	if (mag != 0.f)
	{
		v2x /= mag;
		v2y /= mag;
	}
	float v1x = -v2y;
	float v1y = v2x;
	float mu1 = (0.5*(xx + yy + tmp));
	float mu2 = (0.5*(xx + yy - tmp));
	if (abs(mu1) < abs(mu2))
	{
		//Lambda1 = mu1;
		//Lambda2 = mu2;
		I2[0] = v1x; I2[1] = v1y;
		I1[0] = v2x; I1[1] = v2y;
	}
	else
	{
		//Lambda1 = mu2;
		//Lambda2 = mu1;
		I2[0] = v2x; I2[1] = v2y;
		I1[0] = v1x; I1[1] = v1y;
	}
}
void Application5::principalCurvature(void)
{
	//num of vertices
	int numV = mesh.size();

	//rotation matrices
	vector<vector<vector<float>>> M(numV);
	vector<vector<vector<float>>> Minv(numV);
	for (int i = 0; i < numV; i++)
	{
		M[i].resize(3);
		Minv[i].resize(3);
		for (int j = 0; j < 3; j++)
		{
			M[i][j].resize(3);
			Minv[i][j].resize(3);
		}
	}
	for (int i = 0; i < numV; i++)
	{
		VectorRotationMatrix(mesh[i].Normal, M[i], Minv[i]);
	}

	//neighbors of all vertices
	vector<vector<int>> neighborList(numV);
	vertex_neighbours(neighborList);

	for (int i = 0; i < numV; i++)
	{
		//sort and unique neighbors
		vector<int> nce = neighborList[i];
		for (int j = 0; j < neighborList[i].size(); j++)
		{
			int a = neighborList[i][j];
			vector<int> &second = neighborList[a];
			for (int s = 0; s < second.size(); s++)
			{
				nce.push_back(second[s]);
			}
		}
		sort(nce.begin(), nce.end());
		nce.erase(unique(nce.begin(), nce.end()), nce.end());

		//rotate and make normal
		vector<vector<float>> ve;
		for (int j = 0; j < nce.size(); j++)
		{
			vector<float> temp(3);
			temp[0] = mesh[nce[j]].Vertex[0];
			temp[1] = mesh[nce[j]].Vertex[1];
			temp[2] = mesh[nce[j]].Vertex[2];
			ve.push_back(temp);
		}
		vector<vector<float>> we;
		for (int j = 0; j < ve.size(); j++)
		{
			vector<float> temp(3);
			temp[0] = ve[j][0] * Minv[i][0][0] + ve[j][1] * Minv[i][1][0] + ve[j][2] * Minv[i][2][0];
			temp[1] = ve[j][0] * Minv[i][0][1] + ve[j][1] * Minv[i][1][1] + ve[j][2] * Minv[i][2][1];
			temp[2] = ve[j][0] * Minv[i][0][2] + ve[j][1] * Minv[i][1][2] + ve[j][2] * Minv[i][2][2];
			we.push_back(temp);
		}
		vector<float> f(we.size());
		vector<float> x(we.size());
		vector<float> y(we.size());
		for (int j = 0; j < we.size(); j++)
		{
			f[j] = we[j][0];
			x[j] = we[j][1];
			y[j] = we[j][2];
		}
		//fit patch: f(x,y) = ax^2 + by^2 + cxy + dx + ey + f
		vector<vector<float> >fm(f.size());
		for (int j = 0; j < fm.size(); j++)
		{
			fm[j].resize(6);
			fm[j][0] = x[j] * x[j];
			fm[j][1] = y[j] * y[j];
			fm[j][2] = x[j] * y[j];
			fm[j][3] = x[j];
			fm[j][4] = y[j];
			fm[j][5] = 1.0;
		}
		//find x
		vector<vector<float> >invFM(fm);
		vector<float> abcdef(6);
		CvMat *matrixFM = cvCreateMat(fm.size(), 6, CV_32FC1);
		CvMat *matrixInverse = cvCreateMat(6, fm.size(), CV_32FC1);
		for (int index = 0; index < matrixFM->rows; index++)
			for (int j = 0; j < matrixFM->cols; j++)
				cvmSet(matrixFM, index, j, fm[index][j]);
		cvInvert(matrixFM, matrixInverse, CV_SVD);
		CvMat *bMatrix = cvCreateMat(f.size(), 1, CV_32FC1);
		for (int j = 0; j < f.size(); j++)
			cvmSet(bMatrix, j, 0, f[j]);
		CvMat * result = cvCreateMat(6, 1, CV_32FC1);
		cvMatMul(matrixInverse, bMatrix, result);
		for (int j = 0; j < 6; j++)
			abcdef[j] = cvmGet(result, j, 0);
		float a = abcdef[0];
		float b = abcdef[1];
		float c = abcdef[2];
		//Hessian matrix
		float xx = 2 * a;
		float xy = c;
		float yy = 2 * b;
		vector<float> I1(2);
		vector<float> I2(2);
		eig2(I1, I2, xx, xy, yy);

		mesh[i].MinCurve[0] = I1[0] * M[i][1][0] + I1[1] * M[i][2][0];
		mesh[i].MinCurve[1] = I1[0] * M[i][1][1] + I1[1] * M[i][2][1];
		mesh[i].MinCurve[2] = I1[0] * M[i][1][2] + I1[1] * M[i][2][2];

		mesh[i].MaxCurve[0] = I2[0] * M[i][1][0] + I2[1] * M[i][2][0];
		mesh[i].MaxCurve[1] = I2[0] * M[i][1][1] + I2[1] * M[i][2][1];
		mesh[i].MaxCurve[2] = I2[0] * M[i][1][2] + I2[1] * M[i][2][2];

		if ((I1[0] == 0.f) && (I1[1] == 0.f))
		{
			mesh[i].MinCurve[0] = 0.7071f;
			mesh[i].MinCurve[1] = -0.7071f;
		}
		else NormalNormal(mesh[i].MinCurve);
		if ((I2[0] == 0.f) && (I2[1] == 0.f))
		{
			mesh[i].MaxCurve[0] = -0.7071f;
			mesh[i].MaxCurve[1] = -0.7071f;
		}
		else NormalNormal(mesh[i].MaxCurve);
	}
	return;
}