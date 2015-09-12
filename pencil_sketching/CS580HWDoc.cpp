// CS580HWDoc.cpp : implementation of the CCS580HWDoc class
//

#include "stdafx.h"
#include "CS580HW.h"

#include "CS580HWDoc.h"
#include "CS580HWView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CCS580HWDoc

IMPLEMENT_DYNCREATE(CCS580HWDoc, CDocument)

BEGIN_MESSAGE_MAP(CCS580HWDoc, CDocument)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCS580HWDoc construction/destruction

CCS580HWDoc::CCS580HWDoc()
{
	// TODO: add one-time construction code here

}

CCS580HWDoc::~CCS580HWDoc()
{
}


BOOL CCS580HWDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}
BOOL CCS580HWDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CFrameWnd *pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CCS580HWView *pView = (CCS580HWView *)pFrame->GetActiveView();
	pView->MeshFileInput(wstring(lpszPathName));

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCS580HWDoc serialization

void CCS580HWDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCS580HWDoc diagnostics

#ifdef _DEBUG
void CCS580HWDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCS580HWDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG