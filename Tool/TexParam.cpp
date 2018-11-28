// TexParam.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TexParam.h"
#include "afxdialogex.h"
#include "resource.h"
#include "ListDlg.h"

#include "Texture.h"
#include "Material.h"

struct tRGBA {
	BYTE r, g, b, a;
};

// CTexParam 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTexParam, CDialogEx)

CTexParam::CTexParam(CWnd* pParent /*=NULL*/)
	: CShaderParam(IDD_TEXPARAM, pParent)
	, m_bChange(false)
{

}

CTexParam::~CTexParam()
{
}

void CTexParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editParam);
	DDX_Control(pDX, IDC_EDIT2, m_editData);
	DDX_Control(pDX, IDC_PIC2, m_Pic);
}

void CTexParam::Init(CMaterial * _pMtrl, const tShaderParam & _tParam, UINT INum)
{
	CShaderParam::Init(_pMtrl, _tParam);

	CString str;

	if (m_tParamInfo.strParamName.empty())
	{
		str = L"TEX_";
		str += to_wstring(INum).c_str();
	}
	else
		str = m_tParamInfo.strParamName.c_str();

	m_editParam.SetWindowTextW(str);

	m_pTex = *((CTexture**)m_pMtrl->GetData(m_tParamInfo.eShaderParam));

	if (NULL == m_pTex)
	{
		str = L"";
	}
	else
	{
		str = m_pTex->GetKey().c_str();
	}

	m_editData.SetWindowTextW(str);
	m_bChange = true;
}

BEGIN_MESSAGE_MAP(CTexParam, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CTexParam::OnBnClickedButton)
END_MESSAGE_MAP()


// CTexParam 메시지 처리기입니다.


void CTexParam::OnBnClickedButton()
{
	CListDlg dlg;

	dlg.InitList(RESOURCE_TYPE::TEXTURE);

	CTexture* pTex = (CTexture*)dlg.DoModal();
	m_pTex = pTex;

	if (NULL == pTex)
	{
		m_pMtrl->SetData(m_tParamInfo.eShaderParam, &pTex);
		m_editData.SetWindowTextW(L"");
		return;
	}

	m_pMtrl->SetData(m_tParamInfo.eShaderParam, &pTex);
	m_editData.SetWindowTextW(pTex->GetKey().c_str());

	// 기존 이미지 제거
	m_Image.Destroy();

	// 다시 로드
	CreateImage();

	Invalidate(true);

	OnPaint();

	m_bChange = true;
}

void CTexParam::CreateImage()
{
	CTexture* pTexData = m_pTex;

	CRect rt;
	m_Pic.GetWindowRect(rt);
	ScreenToClient(rt);

	UINT iWidth = (UINT)rt.Width();
	UINT iHeight = (UINT)rt.Height();

	float fTexWidth = pTexData->GetWidth();
	float fTexHeight = pTexData->GetHeight();

	float fXRatio = fTexWidth / (float)iWidth;
	float fYRatio = fTexHeight / (float)iHeight;

	tRGBA* pPixel = (tRGBA*)pTexData->GetPixel();

	m_Image.CreateEx(rt.Width()
		, rt.Height()
		, 32
		, BI_RGB);

	for (UINT i = 0; i < iHeight; ++i)
	{
		for (UINT j = 0; j < iWidth; ++j)
		{
			int iIdx = (int)ceil(((float)i * fYRatio * fTexWidth + (float)j * fXRatio));
			m_Image.SetPixelRGB(j, i, pPixel[iIdx].r, pPixel[iIdx].g, pPixel[iIdx].b);
		}
	}
}

void CTexParam::OnPaint()
{
	CShaderParam::OnPaint();
	CPaintDC dc(this);

	CRect rt;
	m_Pic.GetWindowRect(rt);
	ScreenToClient(rt);

	if (false == m_Image.IsNull())
		m_Image.Draw(dc, rt);
}


void CTexParam::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CShaderParam::PostNcDestroy();
}
