// ResDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "MainFrm.h"
#include "HierachyView.h"
#include "ComponentView.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "ResMgr.h"
#include "ResPtr.h"

#include "Mesh.h"
#include "Material.h"
#include "Prefab.h"
#include "Texture.h"

// CResDlg dialog

IMPLEMENT_DYNAMIC(CResDlg, CDialogEx)

CResDlg::CResDlg(CWnd* pParent /*=NULL*/)
	: CTreeCtrlDlg(IDD_RESDLG, pParent)
	, m_arrResRoot{}
	, m_hHitPrefab(NULL)
{
}

CResDlg::~CResDlg()
	
{
}

void CResDlg::UpdateTreeControl()
{
	CResMgr::GetInst()->SetChange(false);

	HTREEITEM hRoot = m_ctrlTree.GetRootItem();

	while (true)
	{		
		if (NULL == hRoot)
			break;
		DeleteChildren(hRoot);
		hRoot = m_ctrlTree.GetNextItem(hRoot, TVGN_NEXT);
	}

	map<wstring, CResPtr<CMesh>>& mapMesh = CResMgr::GetInst()->GetMeshMap();
	map<wstring, CResPtr<CTexture>>& mapTex = CResMgr::GetInst()->GetTextureMap();
	map<wstring, CResPtr<CPrefab>>& mapPrefab = CResMgr::GetInst()->GetPrefabMap();
	map<wstring, CResPtr<CMaterial>>& mapMtrl = CResMgr::GetInst()->GetMtrlMap();
	vector<CResPtr<CRes>>& vecClone = CResMgr::GetInst()->GetCloneMap();

	for (auto& pair : mapMesh)
		AddItem(pair.second->GetKey().c_str(), m_arrResRoot[(UINT)RES_TYPE::MESH], (DWORD_PTR)((CRes*)pair.second));

	for (auto& pair : mapTex)
		AddItem(pair.second->GetKey().c_str(), m_arrResRoot[(UINT)RES_TYPE::TEXTURE], (DWORD_PTR)((CRes*)pair.second));

	for (auto& pair : mapPrefab)
		AddItem(pair.second->GetKey().c_str(), m_arrResRoot[(UINT)RES_TYPE::PREFAB], (DWORD_PTR)((CRes*)pair.second));

	for (auto& pair : mapMtrl)
		AddItem(pair.second->GetKey().c_str(), m_arrResRoot[(UINT)RES_TYPE::MATERIAL], (DWORD_PTR)((CRes*)pair.second));

	for (auto& pRes : vecClone)
		AddItem(pRes->GetKey().c_str(), m_arrResRoot[(UINT)RES_TYPE::CLONE], (DWORD_PTR)((CRes*)pRes));	

	Invalidate();
}

void CResDlg::Init()
{
	HTREEITEM hRoot = m_ctrlTree.GetRootItem();

	while (true)
	{
		if (NULL == hRoot)
			break;
		DeleteChildren(hRoot);
		hRoot = m_ctrlTree.GetNextItem(hRoot, TVGN_NEXT);
	}

	UpdateTreeControl();
}

void CResDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE2, m_ctrlTree);
}


BEGIN_MESSAGE_MAP(CResDlg, CDialogEx)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_TREE2, &CResDlg::OnNMRClickTree)
	ON_COMMAND(ID_PREFAB_INSTANTIATE, &CResDlg::OnPrefabInstantiate)
	ON_COMMAND(ID_PREFAB_DELETE, &CResDlg::OnPrefabDelete)
END_MESSAGE_MAP()


// CResDlg message handlers


void CResDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (CResMgr::GetInst()->IsChange())
	{
		UpdateTreeControl();
	}

	CTreeCtrlDlg::OnTimer(nIDEvent);
}

BOOL CResDlg::OnInitDialog()
{
	CTreeCtrlDlg::OnInitDialog();

	// TODO:  Add extra initialization here
	SetTimer(0, 1000, NULL);
	
	m_arrResRoot[(UINT)RES_TYPE::MESH] = AddItem(L"Mesh", NULL, NULL);
	m_arrResRoot[(UINT)RES_TYPE::TEXTURE] = AddItem(L"Texture", NULL, NULL);
	m_arrResRoot[(UINT)RES_TYPE::MATERIAL] = AddItem(L"Material", NULL, NULL);
	m_arrResRoot[(UINT)RES_TYPE::PREFAB] = AddItem(L"Prefab", NULL, NULL);
	m_arrResRoot[(UINT)RES_TYPE::CLONE] = AddItem(L"Clone", NULL, NULL);

	UpdateTreeControl();

	ShowScrollBar(SB_HORZ, FALSE);
	ShowScrollBar(SB_VERT, FALSE);

	return TRUE;
}


void CResDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CTreeCtrlDlg::PostNcDestroy();
}


void CResDlg::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint point;

	GetCursorPos(&point);
	m_ctrlTree.ScreenToClient(&point);

	m_hHitPrefab = m_ctrlTree.HitTest(point);

	if (NULL != m_hHitPrefab && m_arrResRoot[(UINT)RES_TYPE::PREFAB] == m_ctrlTree.GetNextItem(m_hHitPrefab, TVGN_PARENT))
	{
		CMenu menu;
		CMenu* pMenu;

		menu.LoadMenuW(IDR_RClick_MENU);
		pMenu = menu.GetSubMenu(1);

		::GetCursorPos(&point);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		menu.DestroyMenu();
	}
	else
	{
		::SetFocus(((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetGameViewHwnd());
	}

	*pResult = 0;
}


void CResDlg::OnPrefabInstantiate()
{
	if (NULL == m_hHitPrefab)
	{
		return;
	} 

	CPrefab* pTargetPrefab = (CPrefab*)m_ctrlTree.GetItemData(m_hHitPrefab);
	CGameObject* pClone = pTargetPrefab->Instantiate();
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->FindLayer(pTargetPrefab->GetLayerName())->AddGameObject(pClone);

	::SetFocus(((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetGameViewHwnd());

	// ComponentView 초기화
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	((CHierachyView*)pMainFrm->GetHierachyView())->Init(TREECTRLDLG::HIERARCHY);
	CComponentView* pView = (CComponentView*)pMainFrm->GetComponentView();
	pView->InitTarget();
	pView->SetTarget(pClone);

	m_hHitPrefab = NULL;
}


void CResDlg::OnPrefabDelete()
{
	if (NULL == m_hHitPrefab)
	{
		return;
	}

	CPrefab* pTargetPrefab = (CPrefab*)m_ctrlTree.GetItemData(m_hHitPrefab);
	CResMgr::GetInst()->DeletePrefab(pTargetPrefab->GetTag());

	::SetFocus(((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetGameViewHwnd());

	// ComponentView 초기화
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	((CHierachyView*)pMainFrm->GetHierachyView())->Init(TREECTRLDLG::RES);
	((CComponentView*)pMainFrm->GetComponentView())->InitTarget();

	m_hHitPrefab = NULL;
}
