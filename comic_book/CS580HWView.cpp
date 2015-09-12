// CS580HWView.cpp : implementation of the CCS580HWView class
//

#include "stdafx.h"
#include "CS580HW.h"

#include "CS580HWDoc.h"
#include "CS580HWView.h"
#include "RotateDlg.h"
#include "TranslateDlg.h"
#include "ScaleDlg.h"

#include "disp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView

IMPLEMENT_DYNCREATE(CCS580HWView, CView)

BEGIN_MESSAGE_MAP(CCS580HWView, CView)
	ON_COMMAND(IDM_RENDER, OnRender)
	ON_COMMAND(IDM_ROTATE, OnRotate)
	ON_COMMAND(IDM_TRANSLATE, OnTranslate)
	ON_COMMAND(IDM_SCALE, OnScale)
	ON_COMMAND(ID_EDIT_POPMATRIX, &CCS580HWView::OnEditPopmatrix)
	ON_COMMAND(ID_EDIT_ADD, &CCS580HWView::OnEditAdd)
	ON_COMMAND(ID_EDIT_DELETELIGHT, &CCS580HWView::OnEditDeletelight)
	ON_COMMAND(ID_ANIMATION, &CCS580HWView::OnAnimation)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView construction/destruction

CCS580HWView::CCS580HWView()
{
	threadrun = false;
	threadpause = false;
	GzNewFrameBuffer(&screen, m_nWidth, m_nHeight);
	req_draw = CreateSemaphore(NULL, 0, 1, NULL);
	allow_draw = CreateSemaphore(NULL, 0, 1, NULL);
	req_render = CreateSemaphore(NULL, 0, 1, NULL);
	allow_render = CreateSemaphore(NULL, 1, 1, NULL);
}

CCS580HWView::~CCS580HWView()
{
}

BOOL CCS580HWView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView drawing

void CCS580HWView::OnDraw(CDC* pDC)
{
	CCS580HWDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	HDC memdc;
	HBITMAP m_bitmap;
	BITMAPINFO binfo;
	BITMAPINFOHEADER &bih = binfo.bmiHeader;

	// Create the bitmap
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biBitCount = 24;
	bih.biWidth = m_nWidth;
	bih.biHeight = -m_nHeight;
	bih.biPlanes = 1;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;

	memdc = ::CreateCompatibleDC(pDC->m_hDC);
	m_bitmap = CreateDIBSection(memdc, &binfo, 0, 0, 0, DIB_RGB_COLORS);
	SelectObject(memdc, m_bitmap);
	SetDIBits(memdc, m_bitmap, 0, m_nHeight, screen, &binfo, DIB_RGB_COLORS);
	BitBlt(pDC->m_hDC, 0, 0, m_nWidth, m_nHeight, memdc, 0, 0, SRCCOPY);
	DeleteDC(memdc);
	DeleteObject(m_bitmap);
}

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView diagnostics

#ifdef _DEBUG
void CCS580HWView::AssertValid() const
{
	CView::AssertValid();
}

void CCS580HWView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCS580HWDoc* CCS580HWView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCS580HWDoc)));
	return (CCS580HWDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCS580HWView message handlers

void CCS580HWView::OnRender()
{
	if (threadrun)
	{
		if (threadpause)
		{
			AfxMessageBox(_T("Paused animation procedure occupies render\n"));
		}
		return;
	}
	// Call renderer

	// Application
	Application5::Render();
	memcpy(screen, m_pFrameBuffer, _msize(screen));

	// Set window size
	CRect clientRect, windowRect;
	int x_offset, y_offset;

	GetClientRect(&clientRect);
	AfxGetMainWnd()->GetWindowRect(&windowRect);

	x_offset = windowRect.Width() - clientRect.Width();
	y_offset = windowRect.Height() - clientRect.Height();

	AfxGetMainWnd()->SetWindowPos(NULL,
		windowRect.left,
		windowRect.top,
		x_offset + m_nWidth,
		y_offset + m_nHeight,
		NULL/*,SWP_SHOWWINDOW*/);

	Invalidate(true);
}

// Callback function for rotation  
void CCS580HWView::OnRotate()
{
	CRotateDlg dlg;
	GzInput* input;
	GzMatrix	rotMat =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	input = m_pUserInput;
	if (input == NULL) return;

	// Initialize
	input->rotation[0] = input->rotation[1] = input->rotation[2] = 0;
	dlg.Initialize(input->rotation[0], input->rotation[1], input->rotation[2]);

	if (dlg.DoModal() == IDOK)
	{
		// Update input rotation value
		input->rotation[dlg.m_nAxis] = dlg.m_fRot;

		//  Create Rotation Matrix 
		switch (dlg.m_nAxis)
		{
		case 0:
			// Create matrix for Rot X
			GzRotXMat(input->rotation[0], rotMat);
			break;
		case 1:
			// Create matrix for Rot Y
			GzRotYMat(input->rotation[1], rotMat);
			break;
		case 2:
			// Create matrix for Rot Z
			GzRotZMat(input->rotation[2], rotMat);
			break;
		}

		// Accumulate matrix
		GzPushMatrix(m_pRender, rotMat);
	}
}

// Callback function for Translation
void CCS580HWView::OnTranslate()
{
	CTranslateDlg dlg;
	GzInput* input;
	GzMatrix	trxMat =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	input = m_pUserInput;
	if (input == NULL) return;

	// Initialize
	input->translation[0] = input->translation[1] = input->translation[2] = 0;
	dlg.Initialize(input->translation[0], input->translation[1], input->translation[2]);

	if (dlg.DoModal() == IDOK)
	{
		// Update input translation value
		input->translation[0] = dlg.m_fTx; input->translation[1] = dlg.m_fTy; input->translation[2] = dlg.m_fTz;

		//  Create Translation Matrix
		GzTrxMat(input->translation, trxMat);

		// Accumulate matrix
		GzPushMatrix(m_pRender, trxMat);
	}
}

// Callback function for Scaling
void CCS580HWView::OnScale()
{
	CScaleDlg dlg;
	GzInput* input;
	GzMatrix scaleMat =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	input = m_pUserInput;
	if (input == NULL) return;

	// Initialize
	input->scale[0] = input->scale[1] = input->scale[2] = 1;
	dlg.Initialize(input->scale[0], input->scale[1], input->scale[2]);

	if (dlg.DoModal() == IDOK)
	{
		// Update input scale value
		input->scale[0] = dlg.m_fSx; input->scale[1] = dlg.m_fSy; input->scale[2] = dlg.m_fSz;

		//  Create Scaling Matrix
		GzScaleMat(input->scale, scaleMat);

		// Accumulate matrix
		GzPushMatrix(m_pRender, scaleMat);
	}
}


void CCS580HWView::OnEditPopmatrix()
{
	if (GzPopMatrix(m_pRender) != GZ_SUCCESS)
	{
		AfxMessageBox(_T("No matrix in stack\n"));
	}
}

void CCS580HWView::OnEditAdd()
{
	static bool sranded = false;
	if (!sranded)
	{
		srand((unsigned int)time(NULL));
		sranded = true;
	}
	const float d2pi = 0.01745329252f;
	float c1 = (rand() & 0xFF) / (float)0xFF;
	float angle1 = (rand() % 360) * d2pi;
	float c2 = (rand() & 0xFF) / (float)0xFF;
	float angle2 = (rand() % 180) * d2pi;
	float c3 = (rand() & 0xFF) / (float)0xFF;
	GzLight light =
	{
		{ sin(angle2) * cos(angle1), sin(angle2) * sin(angle1), cos(angle2) }, { c1, c2, c3 }
	};
	if (GzPushLight(m_pRender, light) != GZ_SUCCESS)
	{
		AfxMessageBox(_T("Too many lights\n"));
	}
}

void CCS580HWView::OnEditDeletelight()
{
	if (GzPopLight(m_pRender) != GZ_SUCCESS)
	{
		AfxMessageBox(_T("No light in environment\n"));
	}
}


DWORD WINAPI ThreadProc1(CCS580HWView *This)
{
	static int direction = 0;
	GzMatrix	rotMat =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	direction = (direction + 1) % 3;
	for (int i = 1; i <= 90; i++)
	{
		while (This->threadpause) Sleep(40);
		switch (direction)
		{
		case 0:
		{
			GzRotXMat((float)(i << 2), rotMat);
			break;
		}
		case 1:
		{
			GzRotYMat((float)(i << 2), rotMat);
			break;
		}
		default:
		{
			GzRotZMat((float)(i << 2), rotMat);
			break;
		}
		}
		GzPushMatrix(This->m_pRender, rotMat);

		ReleaseSemaphore(This->req_render, 1, NULL);
		WaitForSingleObject(This->allow_render, INFINITE);
		This->fastRender();
		WaitForSingleObject(This->req_draw, INFINITE);
		ReleaseSemaphore(This->allow_draw, 1, NULL);

		GzPopMatrix(This->m_pRender);
	}
	This->threadrun = false;
	return 0;
}
DWORD WINAPI ThreadProc2(CCS580HWView *This)
{
	while (This->threadrun)
	{
		ReleaseSemaphore(This->req_draw, 1, NULL);
		WaitForSingleObject(This->allow_draw, INFINITE);
		memcpy(This->screen, This->m_pFrameBuffer, _msize(This->screen));
		WaitForSingleObject(This->req_render, INFINITE);
		ReleaseSemaphore(This->allow_render, 1, NULL);

		This->Invalidate(true);
		Sleep(40);
	}
	return 0;
}
void CCS580HWView::OnAnimation()
{
	if (threadrun)
	{
		threadpause = !threadpause;
		return;
	}
	threadrun = true;
	threadpause = false;

	// Set window size
	CRect clientRect, windowRect;
	int x_offset, y_offset;

	GetClientRect(&clientRect);
	AfxGetMainWnd()->GetWindowRect(&windowRect);

	x_offset = windowRect.Width() - clientRect.Width();
	y_offset = windowRect.Height() - clientRect.Height();

	AfxGetMainWnd()->SetWindowPos(NULL,
		windowRect.left,
		windowRect.top,
		x_offset + m_nWidth,
		y_offset + m_nHeight,
		NULL/*,SWP_SHOWWINDOW*/);

	CloseHandle(CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)ThreadProc1,
		this, 0, NULL));
	CloseHandle(CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)ThreadProc2,
		this, 0, NULL));
}


/* Cancel flicker */
BOOL CCS580HWView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}