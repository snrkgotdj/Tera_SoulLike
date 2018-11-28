// FloatParamDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FloatParamDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "Material.h"

// CFloatParamDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CFloatParamDlg, CDialogEx)

CFloatParamDlg::CFloatParamDlg(CWnd* pParent /*=NULL*/)
	: CShaderParam(IDD_FLOATPARAMDLG, pParent)
	, m_bFocus(false)
{

}

CFloatParamDlg::~CFloatParamDlg()
{
}

void CFloatParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editParam);
	DDX_Control(pDX, IDC_EDIT2, m_editData);
}

void CFloatParamDlg::Init(CMaterial * _pMtrl, const tShaderParam & _tParam, UINT INum)
{
	CShaderParam::Init(_pMtrl, _tParam);

	CString strParam;

	if (m_tParamInfo.strParamName.empty())
	{
		strParam = L"FLOAT_";
		strParam += to_wstring(INum).c_str();
	}
	else
		strParam = m_tParamInfo.strParamName.c_str();

	m_editParam.SetWindowTextW(strParam);
}

void CFloatParamDlg::UpdateParam()
{
	if (NULL == m_pMtrl)
		return;

	CString str;
	float fData = *((float*)m_pMtrl->GetData(m_tParamInfo.eShaderParam));

	str.Format(L"%.2f", fData);
	m_editData.SetWindowTextW(str);
}


BEGIN_MESSAGE_MAP(CFloatParamDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT2, &CFloatParamDlg::OnEnChangeEdit)
	ON_EN_SETFOCUS(IDC_EDIT2, &CFloatParamDlg::OnEnSetfocus)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CFloatParamDlg::OnEnKillfocus)
END_MESSAGE_MAP()


// CFloatParamDlg �޽��� ó�����Դϴ�.


void CFloatParamDlg::OnEnChangeEdit()
{
	CString str;
	m_editData.GetWindowTextW(str);
	float iData = (float)_wtof(str.GetBuffer());

	m_pMtrl->SetData(m_tParamInfo.eShaderParam, &iData);
}


void CFloatParamDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	delete this;
	CShaderParam::PostNcDestroy();
}


void CFloatParamDlg::OnEnSetfocus()
{
	m_bFocus = true;
}


void CFloatParamDlg::OnEnKillfocus()
{
	m_bFocus = false;
}
