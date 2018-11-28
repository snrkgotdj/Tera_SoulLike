// GameView.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "GameView.h"

#include "Scene.h"
#include "SceneMgr.h"
#include "GameObject.h"

#include "MainFrm.h"
#include "ComponentView.h"

#include "DebugMgr.h"
#include "DebugView.h"
#include "AddGameObjectDlg.h"

// CGameView

IMPLEMENT_DYNCREATE(CGameView, CFormView)

CGameView::CGameView()
	: CFormView(IDD_GAMEVIEW)
{

}

CGameView::~CGameView()
{
}

LRESULT CGameView::ReceiveLog(WPARAM _lparam, LPARAM)
{
	wchar_t* pLog = (wchar_t*)_lparam;

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CDebugView* pDebugView = (CDebugView*)pMainFrm->GetDebugView();

	pDebugView->AddLog(_lparam);

	return LRESULT();
}

void CGameView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGameView, CFormView)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(MM_LOG, ReceiveLog)
END_MESSAGE_MAP()


// CGameView diagnostics

#ifdef _DEBUG
void CGameView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CGameView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGameView message handlers


BOOL CGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	//cs.dwExStyle &= WS_EX_CLIENTEDGE;

	return CFormView::PreCreateWindow(cs);
}


void CGameView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (CSceneMgr::GetInst()->IsPlayMode())
		return;

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	if (pMainFrm->IsOnAddObjDlg())
	{
		pMainFrm->GetAddObjDlg()->Instantiate(Vec2((float)point.x, (float)point.y));
		return;
	}

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CGameObject* pObj = pCurScene->FindObject(Vec2((float)point.x, (float)point.y));

	if (NULL != pObj)
	{
		
		CComponentView* pComView = (CComponentView*)pMainFrm->GetComponentView();

		pComView->Init(pObj);
	}

	CDebugMgr::GetInst()->Log(L"Click");

	CFormView::OnLButtonDown(nFlags, point);
}
