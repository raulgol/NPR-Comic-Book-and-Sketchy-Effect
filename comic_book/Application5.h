#include "Application.h"
#include <vector>
using namespace std;

#ifndef APP5HEADER
#define APP5HEADER
class ReadHelper
{
public:
	GzCoord Vertex[3];
	GzCoord Normal[3];
	GzTextureIndex Texture[3];
public:
	ReadHelper() {}
	ReadHelper(const ReadHelper &src)
	{
		memcpy(this, &src, sizeof(ReadHelper));
	}
};
class Application5 : public Application
{
	bool initiated;
	vector<ReadHelper> mesh;
public:
	Application5();
	~Application5() {}

	int fastRender(bool = false);
	virtual int Render();
};
#endif