// ScriptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "ScriptDlg.h"
#include "afxdialogex.h"

#include "GameObject.h"
#include "Script.h"
#include "MgrScript\ScriptMgr.h"

// CScriptDlg dialog

IMPLEMENT_DYNAMIC(CScriptDlg, CDialogEx)

CScriptDlg::CScriptDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_SCRIPTDLG, pParent)
{

}

CScriptDlg::~CScriptDlg()
{
}

void CScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbScript);
	DDX_Control(pDX, IDC_LIST2, m_List);
}

void CScriptDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (NULL == m_pTargetObj || !m_bChange)
		return;

	UpdateList();

	m_bChange = false;
}


BEGIN_MESSAGE_MAP(CScriptDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CScriptDlg::OnAddScript)
	ON_BN_CLICKED(IDC_BUTTON1, &CScriptDlg::OnBnClickedDelete)
END_MESSAGE_MAP()


// CScriptDlg message handlers

void CScriptDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CComponentDlg::PostNcDestroy();
}

void CScriptDlg::OnAddScript()
{
	int iIdx = m_cbScript.GetCurSel();

	if (-1 == iIdx)
		return;

	DWORD dwTypeID = m_cbScript.GetItemData(iIdx);

	CScript* pScript = CScriptMgr::GetScript(dwTypeID);

	m_pTargetObj->AddComponent<CScript>(pScript);

	m_bChange = true;
}


BOOL CScriptDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	vector<tScriptInfo> vec;
	CScriptMgr::GetScriptInfo(vec);

	for (UINT i = 0; i < vec.size(); ++i)
	{
		m_cbScript.InsertString(i, vec[i].strScriptName.c_str());
		m_cbScript.SetItemData(i, vec[i].dwTypeId);
	}

	return TRUE; 
}

void CScriptDlg::UpdateList()
{
	m_List.ResetContent();

	map<SCRIPT_ID, CScript*>& listScript = m_pTargetObj->GetScriptList();
	map<SCRIPT_ID, CScript*>::iterator iter = listScript.begin();
	for (int i = 0; iter != listScript.end(); ++iter, ++i)
	{
		CString strScriptName = CScriptMgr::GetScriptName(iter->second);

		m_List.InsertString(i, strScriptName);
	}
}

void CScriptDlg::OnBnClickedDelete()
{
	int iIdx = m_List.GetCurSel();

	if (-1 == iIdx)
		return;

	if (RET_SUCCESS == m_pTargetObj->DeleteScript(iIdx))
		m_bChange = true;
}
