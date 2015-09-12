#include "rend.h"

#ifndef APPHEADER
#define APPHEADER
class Application
{
protected:
	GzDisplay *m_pDisplay;
	GzInput *m_pUserInput;
public:
	GzRender *m_pRender;
	char *m_pFrameBuffer;
	long m_nWidth;
	long m_nHeight;

public:
	Application();
	virtual ~Application();
	virtual int Render() = 0;
};
#endif