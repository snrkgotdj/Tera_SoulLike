// HierachyView.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "HierachyView.h"

#include "HierachyDlg.h"
#include "ResDlg.h"

// CHierachyView

IMPLEMENT_DYNCREATE(CHierachyView, CFormView)

CHierachyView::CHierachyView()
	: CFormView(IDD_HIERACHYVIEW)
{

}

CHierachyView::~CHierachyView()
{
}

void CHierachyView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHierachyView, CFormView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CHierachyView diagnostics

#ifdef _DEBUG
void CHierachyView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CHierachyView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHierachyView message handlers

void CHierachyView::Init(TREECTRLDLG _eTreeCtrlDlg)
{
	switch (_eTreeCtrlDlg)
	{
	case HIERARCHY:
		m_pHierachyDlg->Init();
		break;
	case RES:
		m_pResDlg->Init();
		break;
	case END:
		m_pHierachyDlg->Init();
		m_pResDlg->Init();
		break;
	default:
		break;
	}
}

int CHierachyView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	RECT rt = {};
	GetWindowRect(&rt);

	// TODO:  Add your specialized creation code here
	m_pHierachyDlg = new CHierachyDlg();
	m_pHierachyDlg->Create(IDD_HIERACHYDLG, this);
	m_pHierachyDlg->SetWindowPos(NULL, 1, 0, rt.right - rt.left - 1, 1070 / 2, 0);
	m_pHierachyDlg->ShowWindow(true);
	m_pHierachyDlg->UpdateWindow();

	m_pResDlg = new CResDlg();
	m_pResDlg->Create(IDD_RESDLG, this);
	m_pResDlg->SetWindowPos(NULL, 1, 1070 / 2, rt.right - rt.left - 1, 1070 / 2, 0);
	m_pResDlg->ShowWindow(true);
	m_pResDlg->UpdateWindow();

	return 0;
}


void CHierachyView::OnDestroy()
{
	CFormView::OnDestroy();
	// TODO: Add your message handler code here
}

void CHierachyView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CSize scrollSize(0, 0);
	SetScrollSizes(MM_TEXT, scrollSize);
}
