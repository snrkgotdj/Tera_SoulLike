// TerrainDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TerrainDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "GameObject.h"
#include "Terrain.h"
#include "MeshRender.h"

// CTerrainDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTerrainDlg, CDialogEx)

CTerrainDlg::CTerrainDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_TERRAINDLG, pParent)
	, m_bBrush1(FALSE)
	, m_bBrush2(FALSE)
	, m_bIsSplateMod(false)
	, m_iYSize(95)
{

}

CTerrainDlg::~CTerrainDlg()
{
}

void CTerrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btnHeightMap);
	DDX_Control(pDX, IDC_BUTTON2, m_btnSplate);
	DDX_Control(pDX, IDC_BUTTON3, m_btnBrush1);
	DDX_Control(pDX, IDC_SLIDER1, m_sliderControl);
	DDX_Control(pDX, IDC_EDIT1, m_editRadius);
	DDX_Check(pDX, IDC_CHECK1, m_bBrush1);
	DDX_Check(pDX, IDC_CHECK2, m_bBrush2);
	DDX_Control(pDX, IDC_BUTTON6, m_btnBrush2);

	int iStartID = IDC_CHECK3;

	for (int i = 0; i < 3; ++i)
	{
		DDX_Check(pDX, iStartID++, m_arrTile[i]);
	}
	DDX_Control(pDX, IDC_BUTTON7, m_btnTile1);
	DDX_Control(pDX, IDC_BUTTON8, m_btnTile2);
	DDX_Control(pDX, IDC_BUTTON9, m_btnTile3);
	DDX_Control(pDX, IDC_BUTTON10, m_btnNaviMesh);
	DDX_Control(pDX, IDC_EDIT6, m_editPatch);
	DDX_Control(pDX, IDC_CHECK6, m_bTess);
}

void CTerrainDlg::UpdateComponent(CGameObject * _pObj)
{
	static bool bOnce = false;
	m_pTargetObj = _pObj;

	if (nullptr == m_pTargetObj)
		return;
	UINT iPatch = m_pTargetObj->Terrain()->GetCurPatch();
	CString	str;
	str.Format(L"%d", iPatch);
	m_editPatch.SetWindowTextW(str);

	if (!bOnce && NULL != m_pTargetObj)
	{
		m_sliderControl.SetPos((int)m_pTargetObj->Terrain()->GetRadiuse());

		m_pTargetObj->Terrain()->SetBrush(0);
		m_bBrush1 = TRUE;
		UpdateData(false);

		OnBnHeightMap();
		bOnce = true;
	}
	else if (NULL == m_pTargetObj || !m_bChange)
		return;

	m_bChange = false;
}

BEGIN_MESSAGE_MAP(CTerrainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CTerrainDlg::OnBnHeightMap)
	ON_BN_CLICKED(IDC_BUTTON2, &CTerrainDlg::OnBnClickedSplat)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CTerrainDlg::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_CHECK1, &CTerrainDlg::OnBnClickedBrush1)
	ON_BN_CLICKED(IDC_CHECK2, &CTerrainDlg::OnBnClickedBrush2)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK3, IDC_CHECK5, &CTerrainDlg::OnBnClickedTile)
	ON_BN_CLICKED(IDC_BUTTON10, &CTerrainDlg::OnBnClickedNaviMesh)
	ON_BN_CLICKED(IDC_CHECK6, &CTerrainDlg::OnBnClickedUseTess)
END_MESSAGE_MAP()


// CTerrainDlg 메시지 처리기입니다.


void CTerrainDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}


void CTerrainDlg::OnBnHeightMap()
{
	m_btnHeightMap.EnableWindow(FALSE);
	m_btnSplate.EnableWindow(TRUE);
	m_btnNaviMesh.EnableWindow(TRUE);

	TERRAIN_MOD eMod = m_pTargetObj->Terrain()->GetMod();
	m_pTargetObj->Terrain()->SetMod(TERRAIN_MOD::HEIGHTMAP_MOD);
	m_pTargetObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_BACK);


	RECT rt = {};
	GetWindowRect(&rt);

	if (eMod == TERRAIN_MOD::SPLAT_MOD)
	{
		rt.bottom -= m_iYSize;
	}
	else if (eMod == TERRAIN_MOD::NAVIMESH_MOD)
	{
		rt.bottom -= m_iYSize * 2;
	}
	else if (eMod == TERRAIN_MOD::END)
	{
		rt.bottom -= m_iYSize * 2;
	}

	MoveWindow(&rt);
}


void CTerrainDlg::OnBnClickedSplat()
{
	m_btnHeightMap.EnableWindow(TRUE);
	m_btnSplate.EnableWindow(FALSE);
	m_btnNaviMesh.EnableWindow(TRUE);

	TERRAIN_MOD eMod = m_pTargetObj->Terrain()->GetMod();
	m_pTargetObj->Terrain()->SetMod(TERRAIN_MOD::SPLAT_MOD);
	m_pTargetObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_BACK);

	RECT rt = {};
	GetWindowRect(&rt);

	if (eMod == TERRAIN_MOD::HEIGHTMAP_MOD)
	{
		rt.bottom += m_iYSize;
	}
	//else if (eMod == TERRAIN_MOD::NAVIMESH_MOD)
	//{
	//	rt.bottom -= m_iYSize;
	//}

	MoveWindow(&rt);
	OnBnClickedTile(0);
}

void CTerrainDlg::OnBnClickedNaviMesh()
{
	m_btnHeightMap.EnableWindow(TRUE);
	m_btnSplate.EnableWindow(TRUE);
	m_btnNaviMesh.EnableWindow(FALSE);

	TERRAIN_MOD eMod = m_pTargetObj->Terrain()->GetMod();
	m_pTargetObj->Terrain()->SetMod(TERRAIN_MOD::NAVIMESH_MOD);
	m_pTargetObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_WIREFRAME);
	RECT rt = {};
	GetWindowRect(&rt);

	if (eMod == TERRAIN_MOD::HEIGHTMAP_MOD)
	{
		rt.bottom += m_iYSize * 2;
	}
	else if(eMod == TERRAIN_MOD::SPLAT_MOD)
	{
		rt.bottom += m_iYSize;
	}
	else if (eMod == TERRAIN_MOD::HEIGHTMAP_REVERSE_MOD)
	{
		rt.bottom += m_iYSize * 2;
	}


	MoveWindow(&rt);
}


BOOL CTerrainDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	m_btnBrush1.LoadBitmaps(IDB_BITMAP1);
	m_btnBrush1.SizeToContent();

	m_btnBrush2.LoadBitmaps(IDB_BITMAP2);
	m_btnBrush2.SizeToContent();

	m_btnTile1.LoadBitmaps(IDB_BITMAP3);
	m_btnTile1.SizeToContent();

	m_btnTile2.LoadBitmaps(IDB_BITMAP4);
	m_btnTile2.SizeToContent();

	m_btnTile3.LoadBitmaps(IDB_BITMAP5);
	m_btnTile3.SizeToContent();

	m_sliderControl.SetRange(1, 10000);
	m_sliderControl.SetTicFreq(10);
	m_sliderControl.SetLineSize(10);

	return TRUE;
}

void CTerrainDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	float fSetRadiuse = (float)m_sliderControl.GetPos();
	m_pTargetObj->Terrain()->SetRadiuse(fSetRadiuse);

	CString	str;
	str.Format(L"%.f", fSetRadiuse);
	m_editRadius.SetWindowTextW(str);

	*pResult = 0;
}


void CTerrainDlg::OnBnClickedBrush1()
{
	m_pTargetObj->Terrain()->SetBrush(0);
	m_bBrush1 = TRUE;
	m_bBrush2 = FALSE;

	UpdateData(false);
}


void CTerrainDlg::OnBnClickedBrush2()
{
	m_pTargetObj->Terrain()->SetBrush(1);
	m_bBrush1 = FALSE;
	m_bBrush2 = TRUE;

	UpdateData(false);
}

void CTerrainDlg::OnBnClickedTile(UINT _id)
{
	int iID = _id - 1011;

	int iIdx = (int)iID % 3;
	for (int i = 0; i < 3; ++i)
	{
		if (i == iIdx)
			m_arrTile[i] = TRUE;
		else
			m_arrTile[i] = FALSE;
	}
	UpdateData(false);

	m_pTargetObj->Terrain()->SetTileIdx(iIdx);
}



void CTerrainDlg::OnBnClickedUseTess()
{
	if (m_bTess.GetCheck())
	{
		m_pTargetObj->Terrain()->SetTessUse(true);
	}
	else
	{
		m_pTargetObj->Terrain()->SetTessUse(false);
	}
}
