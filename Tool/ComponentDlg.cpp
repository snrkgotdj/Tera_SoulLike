#include "stdafx.h"
#include "ComponentDlg.h"


CComponentDlg::CComponentDlg()
	: m_pTargetObj(NULL)
	, m_pPrevTargetObj(NULL)
	, m_bChange(false)
{
}

CComponentDlg::CComponentDlg(UINT nIDTemplate, CWnd * pParent)
	: CDialogEx(nIDTemplate, pParent)
	, m_pTargetObj(NULL)
	, m_pPrevTargetObj(NULL)
	, m_bChange(false)
{
}

CComponentDlg::~CComponentDlg()
{
}

void CComponentDlg::OnCancel()
{	
}

void CComponentDlg::OnOK()
{	
}