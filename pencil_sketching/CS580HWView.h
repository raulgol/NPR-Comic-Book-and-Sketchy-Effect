// CS580HWView.h : interface of the CCS580HWView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Application5.h"

class CCS580HWView : public CView, public Application5
{
public:
	char *screen;
	bool threadrun;
	bool threadpause;
	HANDLE req_draw;
	HANDLE allow_draw;
	HANDLE req_render;
	HANDLE allow_render;
protected: // create from serialization only
	CCS580HWView();
	DECLARE_DYNCREATE(CCS580HWView)

	// Attributes
public:
	CCS580HWDoc* GetDocument();

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCS580HWView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CCS580HWView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CCS580HWView)
	afx_msg void OnRender();
	afx_msg void OnRotate();
	afx_msg void OnTranslate();
	afx_msg void OnScale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEditPopmatrix();
	afx_msg void OnEditDeletelight();
	afx_msg void OnEditAdd();
	afx_msg void OnAnimation();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEditMeshdiv();
	afx_msg void OnMeshSwitcheffect();
	afx_msg void OnMeshTexture();
	afx_msg void OnDemoCar();
	afx_msg void OnDemoF();
	afx_msg void OnDemoPpot();
	afx_msg void OnDemoTorus();
	afx_msg void OnDemoTriceratops();
};

#ifndef _DEBUG  // debug version in CS580HWView.cpp
inline CCS580HWDoc* CCS580HWView::GetDocument()
{ return (CCS580HWDoc*)m_pDocument; }
#endif