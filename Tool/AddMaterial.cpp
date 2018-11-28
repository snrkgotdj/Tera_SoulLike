// AddMaterial.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AddMaterial.h"
#include "afxdialogex.h"
#include "resource.h"

#include "ListDlg.h"

#include "ShaderMgr.h"
#include "Shader.h"
#include "ResMgr.h"
#include "Mesh.h"
#include "Material.h"

// CAddMaterial 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAddMaterial, CDialogEx)

CAddMaterial::CAddMaterial(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADDMATERIAL, pParent)
	, m_pShader(NULL)
{

}

CAddMaterial::~CAddMaterial()
{
}

void CAddMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editName);
	DDX_Control(pDX, IDC_EDIT3, m_editShader);
}


BEGIN_MESSAGE_MAP(CAddMaterial, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAddMaterial::OnBnClickedButton)
	ON_BN_CLICKED(IDC_BUTTON3, &CAddMaterial::OnBnClickedShader)
END_MESSAGE_MAP()


// CAddMaterial 메시지 처리기입니다.

void CAddMaterial::OnBnClickedShader()
{
	CListDlg dlg;

	dlg.InitList(RESOURCE_TYPE::SHADER);

	CShader* pShader = (CShader*)dlg.DoModal();

	m_editShader.SetWindowTextW(pShader->GetKey().c_str());
	m_pShader = pShader;
}

void CAddMaterial::OnBnClickedButton()
{
	CString str;
	m_editName.GetWindowTextW(str);

	if (str.IsEmpty())
		CAddMaterial::OnCancel();

	CMaterial* pMtrl = new CMaterial;
	pMtrl->SetShader(m_pShader);
	CResMgr::GetInst()->AddMtrl(str.GetString(), pMtrl);

	CAddMaterial::OnOK();
}