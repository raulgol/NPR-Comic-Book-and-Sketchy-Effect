#include "stdafx.h"
#include "Application.h"

Application::Application()
{
	m_pDisplay = NULL;
	m_pRender = NULL;
	m_pUserInput = NULL;
	m_pFrameBuffer = NULL;
}

Application::~Application()
{
	if (m_pFrameBuffer != NULL)
	{
		delete m_pFrameBuffer;
		m_pFrameBuffer = NULL;
	}
	if (m_pUserInput != NULL)
	{
		delete m_pUserInput;
		m_pUserInput = NULL;
	}
	if (m_pRender != NULL)
	{
		GzFreeRender(m_pRender);
		m_pRender = NULL;
	}
	if (m_pDisplay != NULL)
	{
		GzFreeDisplay(m_pDisplay);
		m_pDisplay = NULL;
	}
}