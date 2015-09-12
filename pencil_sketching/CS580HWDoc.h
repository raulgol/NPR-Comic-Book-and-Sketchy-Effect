// CS580HWDoc.h : interface of the CCS580HWDoc class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CCS580HWDoc : public CDocument
{
protected: // create from serialization only
	CCS580HWDoc();
	DECLARE_DYNCREATE(CCS580HWDoc)

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// Implementation
public:
	virtual ~CCS580HWDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
