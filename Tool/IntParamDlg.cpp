// IntParamDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "IntParamDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "Material.h"

// CIntParamDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CIntParamDlg, CDialogEx)

CIntParamDlg::CIntParamDlg(CWnd* pParent /*=NULL*/)
	: CShaderParam(IDD_INTPARAMDLG, pParent)
	, m_bFocus(false)
{

}

CIntParamDlg::~CIntParamDlg()
{
}

void CIntParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editParam);
	DDX_Control(pDX, IDC_EDIT2, m_editData);
}

void CIntParamDlg::Init(CMaterial* _pMtrl, const tShaderParam & _tParam, UINT INum)
{
	CShaderParam::Init(_pMtrl, _tParam);

	CString strParam;

	if (m_tParamInfo.strParamName.empty())
	{
		strParam = L"INT_";
		strParam += to_wstring(INum).c_str();
	}
	else
		strParam = m_tParamInfo.strParamName.c_str();

	m_editParam.SetWindowTextW(strParam);
}

void CIntParamDlg::UpdateParam()
{
	if (NULL == m_pMtrl || m_bFocus)
		return;

	CString str;
	int iData = *((int*)m_pMtrl->GetData(m_tParamInfo.eShaderParam));

	str.Format(L"%d", iData);
	m_editData.SetWindowTextW(str);
}


BEGIN_MESSAGE_MAP(CIntParamDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT2, &CIntParamDlg::OnEnChange)
	ON_EN_SETFOCUS(IDC_EDIT2, &CIntParamDlg::OnEnSetfocus)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CIntParamDlg::OnEnKillfocus)
END_MESSAGE_MAP()


// CIntParamDlg �޽��� ó�����Դϴ�.


void CIntParamDlg::OnEnChange()
{
	CString str;
	m_editData.GetWindowTextW(str);
	int iData = _wtoi(str.GetBuffer());

	m_pMtrl->SetData(m_tParamInfo.eShaderParam, &iData);
}

void CIntParamDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	delete this;
	CShaderParam::PostNcDestroy();
}


void CIntParamDlg::OnEnSetfocus()
{
	m_bFocus = true;
}


void CIntParamDlg::OnEnKillfocus()
{
	m_bFocus = false;
}
