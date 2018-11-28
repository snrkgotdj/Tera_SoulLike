// Vec4ParamDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Vec4ParamDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "Material.h"

// CVec4ParamDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CVec4ParamDlg, CDialogEx)

CVec4ParamDlg::CVec4ParamDlg(CWnd* pParent /*=NULL*/)
	: CShaderParam(IDD_VEC4PARAMDLG, pParent)
	, m_bFocus{ false, }
{

}

CVec4ParamDlg::~CVec4ParamDlg()
{
}

void CVec4ParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editParam);

	int iStartID = IDC_X;

	for (int i = 0; i < 4; ++i)
	{
		DDX_Control(pDX, iStartID++, m_arrEdit[i]);
	}
}

void CVec4ParamDlg::Init(CMaterial * _pMtrl, const tShaderParam & _tParam, UINT INum)
{
	CShaderParam::Init(_pMtrl, _tParam);

	CString strParam;

	if (m_tParamInfo.strParamName.empty())
	{
		strParam = L"VEC4_";
		strParam += to_wstring(INum).c_str();
	}
	else
		strParam = m_tParamInfo.strParamName.c_str();

	m_editParam.SetWindowTextW(strParam);
}

void CVec4ParamDlg::UpdateParam()
{
	CString str;

	Vec4 vData = *((Vec4*)m_pMtrl->GetData(m_tParamInfo.eShaderParam));
	float vGetData[4] = { vData.x, vData.y, vData.z, vData.w };

	for (int i = 0; i < 4; ++i)
	{
		if (!m_bFocus[i])
		{
			str.Format(L"%.2f", vGetData[i]);
			m_arrEdit[i].SetWindowTextW(str);
		}
	}
}

void CVec4ParamDlg::OnEnSetfocus(UINT _id)
{
	int iID = _id - IDC_X;
	m_bFocus[iID % 4] = true;
}

void CVec4ParamDlg::OnEnKillfocus(UINT _id)
{
	int iID = _id - IDC_X;
	m_bFocus[iID % 4] = false;
}

BEGIN_MESSAGE_MAP(CVec4ParamDlg, CDialogEx)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_X, IDC_W, &CVec4ParamDlg::OnEnChange)
END_MESSAGE_MAP()


// CVec4ParamDlg 메시지 처리기입니다.
void CVec4ParamDlg::OnEnChange(UINT _id)
{
	int iID = _id - IDC_X;

	if (false == m_bFocus[iID % 4])
		return;

	CString str;
	m_arrEdit[iID % 4].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	Vec4 vData = {};

	if (iID % 4 == 0)
	{
		vData = *((Vec4*)m_pMtrl->GetData(m_tParamInfo.eShaderParam));
		vData.x = f;
	}
	else if (iID % 4 == 1)
	{
		vData = *((Vec4*)m_pMtrl->GetData(m_tParamInfo.eShaderParam));
		vData.y = f;
	}
	else if (iID % 4 == 2)
	{
		vData = *((Vec4*)m_pMtrl->GetData(m_tParamInfo.eShaderParam));
		vData.z = f;
	}
	else if (iID % 4 == 3)
	{
		vData = *((Vec4*)m_pMtrl->GetData(m_tParamInfo.eShaderParam));
		vData.w = f;
	}

	m_pMtrl->SetData(m_tParamInfo.eShaderParam, &vData);
}

void CVec4ParamDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CShaderParam::PostNcDestroy();
}
