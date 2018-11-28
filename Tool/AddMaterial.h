#pragma once
#include "afxwin.h"


// CAddMaterial ��ȭ �����Դϴ�.

class CMesh;
class CShader;

class CAddMaterial : public CDialogEx
{
	DECLARE_DYNAMIC(CAddMaterial)

public:
	CAddMaterial(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAddMaterial();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDMATERIAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CEdit		m_editName;
	CEdit		m_editShader;
	CShader*	m_pShader;

public:
	afx_msg void OnBnClickedButton();
	afx_msg void OnBnClickedShader();
};
