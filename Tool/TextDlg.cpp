// TextDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "TextDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "GameObject.h"
#include "Text.h"

// CTextDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTextDlg, CDialogEx)

CTextDlg::CTextDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_TEXTDLG, pParent)
	, m_bForcusSize(false)
	, m_bForcusColR(false)
	, m_bForcusColG(false)
	, m_bForcusColB(false)
	, m_bForcusText(false)
{

}

CTextDlg::~CTextDlg()
{
}

void CTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editSize);
	DDX_Control(pDX, IDC_EDIT2, m_editColR);
	DDX_Control(pDX, IDC_EDIT3, m_editColG);
	DDX_Control(pDX, IDC_EDIT4, m_editColB);
	DDX_Control(pDX, IDC_EDIT10, m_editText);
}

void CTextDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (NULL == m_pTargetObj || !m_bChange)
		return;

	CText* pText = m_pTargetObj->Text();

	CString str;

	if (!m_bForcusSize)
	{
		str.Format(L"%.2f", pText->GetSize());
		m_editSize.SetWindowTextW(str);
	}

	UINT* arrColorInfo = pText->GetColorInfo();

	if (!m_bForcusColR)
	{
		str.Format(L"%d", arrColorInfo[0]);
		m_editColR.SetWindowTextW(str);
	}

	if (!m_bForcusColG)
	{
		str.Format(L"%d", arrColorInfo[1]);
		m_editColG.SetWindowTextW(str);
	}

	if (!m_bForcusColB)
	{
		str.Format(L"%d", arrColorInfo[2]);
		m_editColB.SetWindowTextW(str);
	}

	if (!m_bForcusText)
	{
		str = pText->GetText().c_str();
		m_editText.SetWindowTextW(str);
	}

	m_bChange = false;
}

BEGIN_MESSAGE_MAP(CTextDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CTextDlg::OnEnChangeSize)
	ON_EN_CHANGE(IDC_EDIT2, &CTextDlg::OnEnChangeColR)
	ON_EN_CHANGE(IDC_EDIT3, &CTextDlg::OnEnChangeColG)
	ON_EN_CHANGE(IDC_EDIT4, &CTextDlg::OnEnChangeColB)
	ON_EN_CHANGE(IDC_EDIT10, &CTextDlg::OnEnChangeText)
	ON_EN_SETFOCUS(IDC_EDIT1, &CTextDlg::OnEnSetfocusSize)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CTextDlg::OnEnKillfocusSize)
	ON_EN_SETFOCUS(IDC_EDIT10, &CTextDlg::OnEnSetfocusText)
	ON_EN_KILLFOCUS(IDC_EDIT10, &CTextDlg::OnEnKillfocusText)
	ON_EN_SETFOCUS(IDC_EDIT2, &CTextDlg::OnEnSetfocusColorR)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CTextDlg::OnEnKillfocusColorR)
	ON_EN_SETFOCUS(IDC_EDIT3, &CTextDlg::OnEnSetfocusColorG)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CTextDlg::OnEnKillfocusColorG)
	ON_EN_SETFOCUS(IDC_EDIT4, &CTextDlg::OnEnSetfocusColorB)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CTextDlg::OnEnKillfocusColorB)
END_MESSAGE_MAP()


// CTextDlg �޽��� ó�����Դϴ�.


void CTextDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	delete this;
	CComponentDlg::PostNcDestroy();
}

void CTextDlg::OnEnChangeSize()
{
	if (false == m_bForcusSize)
		return;

	CString str;
	m_editSize.GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	m_pTargetObj->Text()->SetSize(f);

	m_bChange = true;
}


void CTextDlg::OnEnChangeColR()
{
	if (false == m_bForcusColR)
		return;

	CString str;
	m_editColR.GetWindowTextW(str);
	int i = _wtoi(str.GetBuffer());

	UINT* arrColorInfo = m_pTargetObj->Text()->GetColorInfo();
	arrColorInfo[0] = i;

	m_pTargetObj->Text()->SetColor(m_pTargetObj->Text()->MakeColor(arrColorInfo));

	m_bChange = true;
}


void CTextDlg::OnEnChangeColG()
{
	if (false == m_bForcusColG)
		return;

	CString str;
	m_editColG.GetWindowTextW(str);
	int i = _wtoi(str.GetBuffer());

	UINT* arrColorInfo = m_pTargetObj->Text()->GetColorInfo();
	arrColorInfo[1] = i;

	m_pTargetObj->Text()->SetColor(m_pTargetObj->Text()->MakeColor(arrColorInfo));

	m_bChange = true;
}


void CTextDlg::OnEnChangeColB()
{
	if (false == m_bForcusColB)
		return;

	CString str;
	m_editColB.GetWindowTextW(str);
	int i = _wtoi(str.GetBuffer());

	UINT* arrColorInfo = m_pTargetObj->Text()->GetColorInfo();

	arrColorInfo[2] = i;

	m_pTargetObj->Text()->SetColor(m_pTargetObj->Text()->MakeColor(arrColorInfo));

	m_bChange = true;
}


void CTextDlg::OnEnChangeText()
{
	if (false == m_bForcusText)
		return;

	CString str;
	m_editText.GetWindowTextW(str);
	m_pTargetObj->Text()->SetText(str.GetBuffer());

	m_bChange = true;
}


void CTextDlg::OnEnSetfocusSize()
{
	m_bForcusSize = true;
}


void CTextDlg::OnEnKillfocusSize()
{
	m_bForcusSize = false;
}


void CTextDlg::OnEnSetfocusText()
{
	m_bForcusText = true;
}


void CTextDlg::OnEnKillfocusText()
{
	m_bForcusText = false;
}


void CTextDlg::OnEnSetfocusColorR()
{
	m_bForcusColR = true;
}


void CTextDlg::OnEnKillfocusColorR()
{
	m_bForcusColR = false;
}

void CTextDlg::OnEnSetfocusColorG()
{
	m_bForcusColG = true;
}

void CTextDlg::OnEnKillfocusColorG()
{
	m_bForcusColG = false;
}

void CTextDlg::OnEnSetfocusColorB()
{
	m_bForcusColB = true;
}

void CTextDlg::OnEnKillfocusColorB()
{
	m_bForcusColB = false;
}