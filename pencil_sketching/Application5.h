#include "Application.h"
#include <vector>
using namespace std;

#ifndef APP5HEADER
#define APP5HEADER
class ObjHelper
{
public:
	int totalnorm;
	GzCoord Vertex;
	GzCoord Normal;
	GzCoord MinCurve;
	GzCoord MaxCurve;
	GzTextureIndex Texture;
public:
	ObjHelper() { totalnorm = 0; }
};
class Application5 : public Application
{
	bool initiated;
	vector<ObjHelper> mesh;
	vector<vector<int>> facelist;

	/* principle direction functions */
	int operator+=(ObjHelper &src);
	void vertex_neighbours(vector<vector<int>> &neighborList);
public:

	Application5();
	~Application5() {}

	virtual int Render();
	int fastRender(bool = false);

	void MeshFileInput(wstring fname);
	void MeshCalculation(bool = false);
	void principalCurvature(void);
};
#endif