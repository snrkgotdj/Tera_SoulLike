// LayerSettingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LayerSettingDlg.h"
#include "afxdialogex.h"
#include "Tool.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

// CLayerSettingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLayerSettingDlg, CDialogEx)

CLayerSettingDlg::CLayerSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LAYERSETTINGDLG, pParent)
{

}

CLayerSettingDlg::~CLayerSettingDlg()
{
}

void CLayerSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lcLayer);
	DDX_Control(pDX, IDC_EDIT2, m_editLayerName);
}


BEGIN_MESSAGE_MAP(CLayerSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CLayerSettingDlg::OnBnClickedDeleteLayer)
	ON_BN_CLICKED(IDC_BUTTON2, &CLayerSettingDlg::OnBnClickedAddLayer)
END_MESSAGE_MAP()


// CLayerSettingDlg 메시지 처리기입니다.


BOOL CLayerSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	const vector<CLayer*>& vecLayer = pCurScene->GetVecLayer();

	for (int i = (int)vecLayer.size() -1; i >= 0; --i)
	{
		if (NULL != vecLayer[i])
			m_lcLayer.InsertItem(0, vecLayer[i]->GetLayerName().c_str());
	}

	return TRUE;  				  
}


void CLayerSettingDlg::OnBnClickedDeleteLayer()
{
	POSITION pos = m_lcLayer.GetFirstSelectedItemPosition();

	if (NULL != pos)
	{
		int iIdx = m_lcLayer.GetNextSelectedItem(pos);
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		pCurScene->FindLayer(iIdx)->Destroy();
		m_lcLayer.DeleteItem(iIdx);

	}	
}


void CLayerSettingDlg::OnBnClickedAddLayer()
{
	CString strLayerName;
	m_editLayerName.GetWindowTextW(strLayerName);
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->AddLayer(strLayerName.GetBuffer());

	m_lcLayer.DeleteAllItems();

	const vector<CLayer*>& vecLayer = pCurScene->GetVecLayer();

	for (int i = (int)vecLayer.size() - 1; i >= 0; --i)
	{
		if (NULL != vecLayer[i])
			m_lcLayer.InsertItem(0, vecLayer[i]->GetLayerName().c_str());
	}
}
