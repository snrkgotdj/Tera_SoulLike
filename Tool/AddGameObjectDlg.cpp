// AddGameObjectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AddGameObjectDlg.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "MainFrm.h"

#include "RenderMgr.h"
#include "ResMgr.h"
#include "Prefab.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Terrain.h"

// CAddGameObjectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAddGameObjectDlg, CDialogEx)

CAddGameObjectDlg::CAddGameObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADDGAMEOBJECTDLG, pParent)
{

}

CAddGameObjectDlg::~CAddGameObjectDlg()
{
}

void CAddGameObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_listboxPrefab);
}


BEGIN_MESSAGE_MAP(CAddGameObjectDlg, CDialogEx)
END_MESSAGE_MAP()


// CAddGameObjectDlg 메시지 처리기입니다.


BOOL CAddGameObjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	map<wstring, CResPtr<CPrefab>> mapPrefab = CResMgr::GetInst()->GetPrefabMap();
	map<wstring, CResPtr<CPrefab>>::iterator iter = mapPrefab.begin();

	for (UINT i = 0; iter != mapPrefab.end(); ++iter, ++i)
	{
		m_listboxPrefab.InsertString(i, iter->first.c_str());
		m_listboxPrefab.SetItemData(i, (DWORD_PTR)iter->second.GetTarget());
	}

	return TRUE;  				  
}

void CAddGameObjectDlg::Instantiate(const Vec2& _vWindowPos)
{
	int iCurSel = m_listboxPrefab.GetCurSel();
	if (iCurSel < 0)
		return;

	Vec3 vPos = {};

	CGameObject* pTerrainObj = CSceneMgr::GetInst()->GetTerrainObj();
	if (RET_FAILED == pTerrainObj->Terrain()->GetWorldPosbyTex(vPos))
		return;

	CPrefab* pPrefab = (CPrefab*)m_listboxPrefab.GetItemData(iCurSel);
	CGameObject* pObj = pPrefab->Instantiate();
	CSceneMgr::GetInst()->GetCurScene()->FindLayer(pPrefab->GetLayerName())->AddGameObject(pObj);
	pObj->Transform()->SetLocalPos(vPos);
}


void CAddGameObjectDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMainFrm->SetOnAddObjDlg(false);
	CDialogEx::OnCancel();
}
