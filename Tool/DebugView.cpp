// DebugView.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "DebugView.h"

#include "DebugMgr.h"
// CDebugView

IMPLEMENT_DYNCREATE(CDebugView, CFormView)

CDebugView::CDebugView()
	: CFormView(IDD_DEBUGVIEW)
{

}

CDebugView::~CDebugView()
{
}

void CDebugView::AddLog(DWORD_PTR _pLog)
{
	int iCount = m_LogBox.GetCount();	
	m_LogBox.InsertString(iCount, (LPCTSTR)_pLog);
}

void CDebugView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_LogBox);
}

BEGIN_MESSAGE_MAP(CDebugView, CFormView)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDebugView diagnostics

#ifdef _DEBUG
void CDebugView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDebugView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDebugView message handlers


HBRUSH CDebugView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	CRect rect;	
	if (pWnd->GetDlgCtrlID() == IDC_LIST1)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		hbr = m_Brush;
	}		

	return hbr;
}


int CDebugView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	m_Brush.CreateSolidBrush(RGB(120, 120, 120));   // Blue brush.
	

	return 0;
}


void CDebugView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	static bool bInit = false;

	if (bInit)
		return;	
	bInit = true;
	
}