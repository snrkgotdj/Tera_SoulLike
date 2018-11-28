// ComListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "ComListDlg.h"
#include "afxdialogex.h"


// CComListDlg dialog

IMPLEMENT_DYNAMIC(CComListDlg, CDialogEx)

CComListDlg::CComListDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_COMLISTDLG, pParent)
{

}

CComListDlg::~CComListDlg()
{
}

void CComListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbCom);
}


BEGIN_MESSAGE_MAP(CComListDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CComListDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CComListDlg message handlers


void CComListDlg::OnBnClickedOk()
{
	int iSel = m_cbCom.GetCurSel();
	if (-1 == iSel)
		EndDialog(-1);
	else
	{
		switch (iSel)
		{
		case 0:
			iSel = (UINT)COMPONENT_TYPE::TRANSFORM;
			break;
		case 1:
			iSel = (UINT)COMPONENT_TYPE::MESHRENDER;
			break;
		case 2:
			iSel = (UINT)COMPONENT_TYPE::COLLIDER_2D;
			break;
		case 3:
			iSel = (UINT)COMPONENT_TYPE::COLLIDER_3D;
			break;
		case 4:
			iSel = (UINT)COMPONENT_TYPE::ANIMATOR_2D;
			break;
		case 5:
			iSel = (UINT)COMPONENT_TYPE::ANIMATOR_3D;
			break;
		case 6:
			iSel = (UINT)COMPONENT_TYPE::CAMERA;
			break;
		case 7:
			iSel = (UINT)COMPONENT_TYPE::LIGHT;
			break;	
		case 8:
			iSel = (UINT)COMPONENT_TYPE::TRAIL;
			break;
		}		
		EndDialog(iSel);
	}	
}


BOOL CComListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
		
	m_cbCom.InsertString(0, L"TRANSFORM");
	m_cbCom.InsertString(1, L"MESHRENDER");
	m_cbCom.InsertString(2, L"COLLIDER_2D");
	m_cbCom.InsertString(3, L"COLLIDER_3D");
	m_cbCom.InsertString(4, L"ANIMATOR_2D");
	m_cbCom.InsertString(5, L"ANIMATOR_2D");
	m_cbCom.InsertString(6, L"CAMERA");
	m_cbCom.InsertString(7, L"LIGHT");	
	m_cbCom.InsertString(8, L"TRAIL");

	return TRUE;  				  
}


void CComListDlg::OnCancel()
{
	EndDialog(-1);
}
