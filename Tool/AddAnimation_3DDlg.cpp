// AddAnimation_3DDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "AddAnimation_3DDlg.h"
#include "afxdialogex.h"
#include "Tool.h"

#include "GameObject.h"
#include "Animator_3D.h"

// CAddAnimation_3DDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAddAnimation_3DDlg, CDialogEx)

CAddAnimation_3DDlg::CAddAnimation_3DDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADDANIMATION_3DDLG, pParent)
	, m_pTargetObj(NULL)
{

}

CAddAnimation_3DDlg::~CAddAnimation_3DDlg()
{
}

void CAddAnimation_3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editKey);
	DDX_Control(pDX, IDC_EDIT2, m_editStart);
	DDX_Control(pDX, IDC_EDIT3, m_editEnd);
}


BEGIN_MESSAGE_MAP(CAddAnimation_3DDlg, CDialogEx)
END_MESSAGE_MAP()


// CAddAnimation_3DDlg �޽��� ó�����Դϴ�.


void CAddAnimation_3DDlg::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(NULL == m_pTargetObj)
		CDialogEx::OnOK();

	wstring strKey;
	UINT iStartFrame;
	UINT iEndFrame;

	CString str;
	m_editKey.GetWindowTextW(str);
	strKey = str.GetBuffer();

	m_editStart.GetWindowTextW(str);
	iStartFrame = _wtoi(str);

	m_editEnd.GetWindowTextW(str);
	iEndFrame = _wtoi(str);

	m_pTargetObj->Animator_3D()->AddAnimation(strKey, iStartFrame, iEndFrame);

	CDialogEx::OnOK();
}
