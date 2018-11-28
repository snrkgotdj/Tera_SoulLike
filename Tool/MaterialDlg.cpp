// MaterialDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MaterialDlg.h"
#include "afxdialogex.h"

#include "ListDlg.h"
#include "IntParamDlg.h"
#include "FloatParamDlg.h"
#include "Vec4ParamDlg.h"
#include "TexParam.h"

#include "MeshRender.h"
#include "Material.h"
#include "Shader.h"


// CMaterialDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMaterialDlg, CDialogEx)

CMaterialDlg::CMaterialDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MATERIALDLG, pParent)
	, m_pMR(NULL)
	, m_pMtrl(NULL)
	, m_lStartPosY(0)
	, m_iIdx(0)
	, m_bIsOpen(false)
{

}

CMaterialDlg::~CMaterialDlg()
{
}

void CMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editMtrl);
	DDX_Control(pDX, IDC_EDIT2, m_editShader);
	DDX_Control(pDX, IDC_BUTTON1, m_Button);
}


BEGIN_MESSAGE_MAP(CMaterialDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CMaterialDlg::OnBnClickedMtrl)
	ON_BN_CLICKED(IDC_BUTTON7, &CMaterialDlg::OnBnClickedShader)
	ON_BN_CLICKED(IDC_BUTTON1, &CMaterialDlg::OnBnClickedButton)
END_MESSAGE_MAP()


BOOL CMaterialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	RECT rt = {};
	GetWindowRect(&rt);

	return TRUE;
}


// CMaterialDlg 메시지 처리기입니다.

void CMaterialDlg::InitMaterial(CMeshRender * _pMR, UINT _iIdx)
{
	Close();

	Safe_Delete_Vec(m_vecParam);
	m_vecParam.clear();

	m_pMR = _pMR;
	m_iIdx = _iIdx;
	m_pMtrl = m_pMR->GetSharedMaterial(m_iIdx);

	RECT rt = {};
	GetWindowRect(&rt);
	m_lStartPosY = rt.bottom - rt.top;

	CString strMtrlKey = L"";
	CString strShaderKey = L"";

	if (NULL != m_pMtrl)
	{
		strMtrlKey = m_pMtrl->GetKey().c_str();

		CShader* pShader = m_pMtrl->GetShader();

		if (NULL == pShader)
			return;

		vector<tShaderParam>& vecParam = pShader->GetShaderParam();

		for (UINT i = 0; i < vecParam.size(); ++i)
		{
			InitShaderParam(vecParam[i]);
		}

		if (vecParam.empty())
			m_Button.ShowWindow(false);
		else
			m_Button.ShowWindow(true);

		strShaderKey = pShader->GetKey().c_str();;
	}

	m_editShader.SetWindowTextW(strShaderKey);
	m_editMtrl.SetWindowTextW(strMtrlKey);
}

void CMaterialDlg::UpdateParam()
{
	if (false == m_bIsOpen)
		return;

	for (UINT i = 0; i < m_vecParam.size(); ++i)
	{
		m_vecParam[i]->UpdateParam();
	}
}

void CMaterialDlg::InitShaderParam(const tShaderParam & _tParam)
{
	RECT rt = {};
	GetWindowRect(&rt);
	RECT rtMtrl = {};

	switch (_tParam.eShaderParam)
	{
	case SHADER_PARAM::SP_INT_0:
	case SHADER_PARAM::SP_INT_1:
	case SHADER_PARAM::SP_INT_2:
	case SHADER_PARAM::SP_INT_3:
	case SHADER_PARAM::SP_INT_4:
	case SHADER_PARAM::SP_INT_5:
	case SHADER_PARAM::SP_INT_6:
	case SHADER_PARAM::SP_INT_7:
	{
		CIntParamDlg* pParam = new CIntParamDlg;
		pParam->Create(IDD_INTPARAMDLG, this);
		pParam->ShowWindow(true);
		pParam->UpdateWindow();

		pParam->Init(m_pMtrl, _tParam, (UINT)_tParam.eShaderParam - (UINT)SHADER_PARAM::SP_INT_0);

		pParam->GetWindowRect(&rtMtrl);
		pParam->SetWindowPos(NULL, 0, m_lStartPosY, rt.right - rt.left, rtMtrl.bottom - rtMtrl.top, 0);

		m_lStartPosY += rtMtrl.bottom - rtMtrl.top;
		m_vecParam.push_back(pParam);
	}
	break;
	case SHADER_PARAM::SP_FLOAT_0:
	case SHADER_PARAM::SP_FLOAT_1:
	case SHADER_PARAM::SP_FLOAT_2:
	case SHADER_PARAM::SP_FLOAT_3:
	case SHADER_PARAM::SP_FLOAT_4:
	case SHADER_PARAM::SP_FLOAT_5:
	case SHADER_PARAM::SP_FLOAT_6:
	case SHADER_PARAM::SP_FLOAT_7:
	{
		CFloatParamDlg* pParam = new CFloatParamDlg;
		pParam->Create(IDD_FLOATPARAMDLG, this);
		pParam->ShowWindow(true);
		pParam->UpdateWindow();

		pParam->Init(m_pMtrl, _tParam, (UINT)_tParam.eShaderParam - (UINT)SHADER_PARAM::SP_TEX_0);

		pParam->GetWindowRect(&rtMtrl);
		pParam->SetWindowPos(NULL, 0, m_lStartPosY, rt.right - rt.left, rtMtrl.bottom - rtMtrl.top, 0);

		m_lStartPosY += rtMtrl.bottom - rtMtrl.top;
		m_vecParam.push_back(pParam);
	}
	break;
	case SHADER_PARAM::SP_VEC4_0:
	case SHADER_PARAM::SP_VEC4_1:
	case SHADER_PARAM::SP_VEC4_2:
	case SHADER_PARAM::SP_VEC4_3:
	case SHADER_PARAM::SP_VEC4_4:
	case SHADER_PARAM::SP_VEC4_5:
	case SHADER_PARAM::SP_VEC4_6:
	case SHADER_PARAM::SP_VEC4_7:
	{
		CVec4ParamDlg* pParam = new CVec4ParamDlg;
		pParam->Create(IDD_VEC4PARAMDLG, this);
		pParam->ShowWindow(true);
		pParam->UpdateWindow();

		pParam->Init(m_pMtrl, _tParam, (UINT)_tParam.eShaderParam - (UINT)SHADER_PARAM::SP_TEX_0);

		pParam->GetWindowRect(&rtMtrl);
		pParam->SetWindowPos(NULL, 0, m_lStartPosY, rt.right - rt.left, rtMtrl.bottom - rtMtrl.top, 0);

		m_lStartPosY += rtMtrl.bottom - rtMtrl.top;
		m_vecParam.push_back(pParam);
	}
	break;
	case SHADER_PARAM::SP_TEX_0:
	case SHADER_PARAM::SP_TEX_1:
	case SHADER_PARAM::SP_TEX_2:
	case SHADER_PARAM::SP_TEX_3:
	case SHADER_PARAM::SP_TEX_4:
	case SHADER_PARAM::SP_TEX_5:
	case SHADER_PARAM::SP_TEX_6:
	case SHADER_PARAM::SP_TEX_7:
	case SHADER_PARAM::SP_TEXARR_0:
	case SHADER_PARAM::SP_TEXARR_1:
	case SHADER_PARAM::SP_TEXARR_2:
	case SHADER_PARAM::SP_TEXARR_3:
	case SHADER_PARAM::SP_TEXARR_4:
	case SHADER_PARAM::SP_TEXARR_5:
	case SHADER_PARAM::SP_TEXARR_6:
	case SHADER_PARAM::SP_TEXARR_7:
	{
		CTexParam* pParam = new CTexParam(this);
		pParam->Create(IDD_TEXPARAM, this);
		pParam->ShowWindow(true);

		pParam->GetWindowRect(&rtMtrl);
		pParam->SetWindowPos(NULL, 0, m_lStartPosY, rt.right - rt.left, rtMtrl.bottom - rtMtrl.top, 0);
		m_lStartPosY += rtMtrl.bottom - rtMtrl.top;

		pParam->Init(m_pMtrl, _tParam, (UINT)_tParam.eShaderParam - (UINT)SHADER_PARAM::SP_TEX_0);
		pParam->UpdateWindow();

		m_vecParam.push_back(pParam);
	}
	break;
	default:
		break;
	}
}

void CMaterialDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CDialogEx::PostNcDestroy();
}


void CMaterialDlg::OnBnClickedMtrl()
{
	CListDlg dlg;

	dlg.InitList(RESOURCE_TYPE::MATERIAL);

	CMaterial* pMtrl = (CMaterial*)dlg.DoModal();

	if (NULL == pMtrl)
		return;

	m_pMR->SetMaterial(pMtrl, m_iIdx);
	InitMaterial(m_pMR, m_iIdx);
}


void CMaterialDlg::OnBnClickedShader()
{
	CListDlg dlg;

	dlg.InitList(RESOURCE_TYPE::SHADER);

	CShader* pShader = (CShader*)dlg.DoModal();

	if (NULL == pShader)
		return;

	m_pMtrl->SetShader(pShader);
	m_editShader.SetWindowTextW(pShader->GetKey().c_str());

	vector<tShaderParam>& vecParam = pShader->GetShaderParam();

	for (UINT i = 0; i < vecParam.size(); ++i)
	{
		InitShaderParam(vecParam[i]);
	}
}

#include "MeshRenderDlg.h"
void CMaterialDlg::OnBnClickedButton()
{
	RECT rtParent = {};
	GetParent()->GetWindowRect(&rtParent);

	RECT rt = {};
	GetWindowRect(&rt);

	if (m_bIsOpen)
	{
		RECT rtOpen = {};
		m_vecParam.front()->GetWindowRect(&rtOpen);

		RECT rtOther = {};
		bool bIsThis = false;
		int iCount = 0;
		CMeshRenderDlg* pMRDlg = (CMeshRenderDlg*)GetParent();
		for (UINT i = 0; i < pMRDlg->m_vecMtrlDlg.size(); ++i)
		{
			if (pMRDlg->m_vecMtrlDlg[i] == this)
			{
				bIsThis = true;
				SetWindowPos(NULL, 0, rt.top - rtParent.top, rt.right - rt.left, rtOpen.top - rt.top, SWP_NOMOVE);
				GetWindowRect(&rt);
				continue;
			}

			if (bIsThis)
			{
				RECT rtPrev = {};
				pMRDlg->m_vecMtrlDlg[i - 1]->GetWindowRect(&rtPrev);

				pMRDlg->m_vecMtrlDlg[i]->GetWindowRect(&rtOther);
				pMRDlg->m_vecMtrlDlg[i]->SetWindowPos(NULL, 0, rtPrev.bottom - rtParent.top - 2, rtOther.right - rtOther.left, rtOther.bottom - rtOther.top, 0);
				pMRDlg->m_vecMtrlDlg[i]->GetWindowRect(&rtOther);
				iCount++;
			}
		}
		if(iCount)
			GetParent()->SetWindowPos(NULL, 0, 0, rtParent.right - rtParent.left, rtOther.bottom - rtParent.top, SWP_NOMOVE);
		else
			GetParent()->SetWindowPos(NULL, 0, 0, rtParent.right - rtParent.left, rt.bottom - rtParent.top, SWP_NOMOVE);


		m_bIsOpen = false;
		m_Button.SetWindowTextW(L"▼");
	}
	else if (m_vecParam.size() > 0)
	{
		RECT rtOpen = {};
		m_vecParam.back()->GetWindowRect(&rtOpen);

		RECT rtOther = {};
		bool bIsThis = false;
		int iCount = 0;
		CMeshRenderDlg* pMRDlg = (CMeshRenderDlg*)GetParent();
		for (UINT i = 0; i < pMRDlg->m_vecMtrlDlg.size(); ++i)
		{
			if (pMRDlg->m_vecMtrlDlg[i] == this)
			{
				bIsThis = true;
				SetWindowPos(NULL, 0, rt.top - rtParent.top, rt.right - rt.left, rtOpen.bottom - rt.top, SWP_NOMOVE);
				continue;
			}

			if (bIsThis)
			{		
				LONG lPosY = 0;
				RECT rtPrev = {};
				pMRDlg->m_vecMtrlDlg[i - 1]->GetWindowRect(&rtPrev);
				lPosY = rtPrev.bottom - rtParent.top - 2;
				pMRDlg->m_vecMtrlDlg[i]->GetWindowRect(&rtOther);
				pMRDlg->m_vecMtrlDlg[i]->SetWindowPos(NULL, 0, lPosY, rtOther.right - rtOther.left, rtOther.bottom - rtOther.top, 0);
				iCount++;
			}
		}
		pMRDlg->m_vecMtrlDlg.back()->GetWindowRect(&rtOther);
		GetParent()->SetWindowPos(NULL, 0, 0, rtParent.right - rtParent.left, rtOther.bottom - rtParent.top, SWP_NOMOVE);
		m_bIsOpen = true;
		m_Button.SetWindowTextW(L"▲");
	}
	else
	{
		return;
	}
}

void CMaterialDlg::Close()
{
	if (m_bIsOpen)
		OnBnClickedButton();
}