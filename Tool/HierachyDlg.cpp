// HierachyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HierachyDlg.h"
#include "afxdialogex.h"

#include "resource.h"

#include "ResMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "MainFrm.h"
#include "ComponentView.h"
#include "HierachyView.h"

#include "DebugMgr.h"
#include "../External/SaveLoadMgr.h"

// CHierachyDlg dialog

IMPLEMENT_DYNAMIC(CHierachyDlg, CDialogEx)

CHierachyDlg::CHierachyDlg(CWnd* pParent /*=NULL*/)
	: CTreeCtrlDlg(IDD_HIERACHYDLG, pParent)
	, m_hDragItem(NULL)
{
}

CHierachyDlg::~CHierachyDlg()
{
}

void CHierachyDlg::UpdateTreeControl()
{
	m_ctrlTree.DeleteAllItems();

	// 모든 게임오브젝트 가져옴
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	list<CGameObject*> listObj;
	pCurScene->GetAllParentObjects(listObj);
	
	list<CGameObject*>::iterator iter = listObj.begin();
	for (; iter != listObj.end(); ++iter)
	{
		AddGameObject((*iter), NULL);
	}

	Invalidate();
}

void CHierachyDlg::Init()
{
	UpdateTreeControl();
}

void CHierachyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ctrlTree);
}


BEGIN_MESSAGE_MAP(CHierachyDlg, CDialogEx)
	ON_WM_TIMER()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CHierachyDlg::OnTvnSelchangedTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE1, &CHierachyDlg::OnTvnBegindragTree)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_HIERARACY_DELETE, &CHierachyDlg::OnObjectDelete)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CHierachyDlg::OnNMRClickTree)
	ON_COMMAND(ID_HIERARACY_ADDPREFAB, &CHierachyDlg::OnAddprefab)
	ON_COMMAND(ID_HIERARACY_SAVEGAMEOBJ, &CHierachyDlg::OnSavegameobj)
END_MESSAGE_MAP()


// CHierachyDlg message handlers
void CHierachyDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialogEx::PostNcDestroy();
}

BOOL CHierachyDlg::OnInitDialog()
{
	CTreeCtrlDlg::OnInitDialog();
	
	SetTimer(0, 100, NULL);
		
	return TRUE; 
}

void CHierachyDlg::AddGameObject(CGameObject * _pObj, HTREEITEM _hParent)
{
	// 해당 아이템을 트리컨트롤에 넣는다.
	HTREEITEM hItem = AddItem(_pObj->GetTag(), _hParent, (DWORD_PTR)_pObj);

	list<CGameObject*>& listchild = _pObj->GetChildList();
	list<CGameObject*>::iterator iter = listchild.begin();
	for (; iter != listchild.end(); ++iter)
	{
		AddGameObject((*iter), hItem);
	}
}

void CHierachyDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

#ifdef DEBUG
	if (CSceneMgr::GetInst()->GetUpdateCheck())
	{
		CSceneMgr::GetInst()->SetUpdateCheck(false);

		// Tree Control 을 GameObject 정보로 갱신한다.
		UpdateTreeControl();
	}
#endif

	CTreeCtrlDlg::OnTimer(nIDEvent);
}

void CHierachyDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if (NULL != m_hDragItem)
		return;

	CGameObject* pObj = (CGameObject*)m_ctrlTree.GetItemData(pNMTreeView->itemNew.hItem);
	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CComponentView* pComView = (CComponentView*)pMainFrm->GetComponentView();

	pComView->Init(pObj);
	
	*pResult = 0;
}


void CHierachyDlg::OnTvnBegindragTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	m_hDragItem = pNMTreeView->itemNew.hItem;

	// 마우스 이벤트 독점
	SetCapture();

	

	*pResult = 0;
}


void CHierachyDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (NULL == m_hDragItem)
		return;

	// 드래그 중일 경우
	::ClientToScreen(m_hWnd, &point);
	m_ctrlTree.ScreenToClient(&point);

	m_hTargetItem = m_ctrlTree.HitTest(point);
	
	if (NULL != m_hTargetItem)
	{
		m_ctrlTree.SelectDropTarget(m_hTargetItem);
	}

	CTreeCtrlDlg::OnMouseMove(nFlags, point);
}


void CHierachyDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (NULL == m_hDragItem)
		return;

	// 마우스 캡쳐 해제
	ReleaseCapture();

	// 하이라이트 제거
	m_ctrlTree.SelectDropTarget(NULL);

	// 목적지가 자기자신이거나, 목적지가 자신의 부모일 경우
	if (m_hDragItem == m_hTargetItem
		|| m_hTargetItem == m_ctrlTree.GetNextItem(m_hDragItem, TVGN_PARENT))
	{
		m_hDragItem = NULL;
		m_hTargetItem = NULL;
		return;
	}

	CGameObject* pDragObj = (CGameObject*)m_ctrlTree.GetItemData(m_hDragItem);

	if (NULL == m_hTargetItem)
	{
		pDragObj->ClearParent();
		CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(pDragObj->GetLayerName());
		pLayer->AddParentObject(pDragObj);
		MoveTreeItem(m_hDragItem, m_hTargetItem);
	}
	else
	{
		CGameObject* pTargetObj = (CGameObject*)m_ctrlTree.GetItemData(m_hTargetItem);
		if (!FAILED(pTargetObj->AddChildObj(pDragObj)))
		{
			MoveTreeItem(m_hDragItem, m_hTargetItem);
		}
	}	

	// 초기화
	m_hDragItem = NULL;
	m_hTargetItem = NULL;
	
	CTreeCtrlDlg::OnLButtonUp(nFlags, point);
}

void CHierachyDlg::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint point;

	GetCursorPos(&point);
	m_ctrlTree.ScreenToClient(&point);

	m_hHitEventTarget = m_ctrlTree.HitTest(point);

	if (NULL != m_hHitEventTarget)
	{
		CMenu menu;
		CMenu* pMenu;

		menu.LoadMenuW(IDR_RClick_MENU);
		pMenu = menu.GetSubMenu(0);

		::GetCursorPos(&point);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		menu.DestroyMenu();
	}

	*pResult = 0;
}

void CHierachyDlg::OnObjectDelete()
{
	if (NULL == m_hHitEventTarget)
	{
		return;
	}

	CGameObject* pTargetObj = (CGameObject*)m_ctrlTree.GetItemData(m_hHitEventTarget);
	pTargetObj->Destroy();

	// ComponentView 초기화
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	((CHierachyView*)pMainFrm->GetHierachyView())->Init(TREECTRLDLG::HIERARCHY);
	((CComponentView*)pMainFrm->GetComponentView())->InitTarget();

	m_hHitEventTarget = NULL;
}

void CHierachyDlg::OnAddprefab()
{
	if (NULL == m_hHitEventTarget)
	{
		return;
	}

	CGameObject* pTargetObj = (CGameObject*)m_ctrlTree.GetItemData(m_hHitEventTarget);
	CResMgr::GetInst()->AddPrefab(pTargetObj->GetTag(), pTargetObj->GetLayerName(),  pTargetObj->Clone());

	// ComponentView 초기화
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	((CHierachyView*)pMainFrm->GetHierachyView())->Init(TREECTRLDLG::RES);
	((CComponentView*)pMainFrm->GetComponentView())->InitTarget();

	m_hHitEventTarget = NULL;
}


void CHierachyDlg::OnSavegameobj()
{
	CGameObject* pTargetObj = (CGameObject*)m_ctrlTree.GetItemData(m_hHitEventTarget);
	if (nullptr == pTargetObj)
	{
		MessageBox(L"세이브 실패", L"게임오브젝트를 선택하세요");
		return;
	}
	wchar_t szFilter[] = L"Scene File (*.goc) |*.goc;|";

	CFileDialog dlg(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT, szFilter);
	CString strInitPath = CPathMgr::GetResPath();
	strInitPath += L"Save\\GameObject";
	dlg.m_ofn.lpstrInitialDir = strInitPath;

	if (IDOK == dlg.DoModal())
	{
		CString strPathName = dlg.GetPathName();
		CSaveLoadMgr::GetInst()->SaveGameObject(strPathName.GetBuffer(), pTargetObj);
	}
}
