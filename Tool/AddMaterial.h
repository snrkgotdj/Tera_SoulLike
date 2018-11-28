#pragma once
#include "afxwin.h"


// CAddMaterial 대화 상자입니다.

class CMesh;
class CShader;

class CAddMaterial : public CDialogEx
{
	DECLARE_DYNAMIC(CAddMaterial)

public:
	CAddMaterial(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddMaterial();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDMATERIAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CEdit		m_editName;
	CEdit		m_editShader;
	CShader*	m_pShader;

public:
	afx_msg void OnBnClickedButton();
	afx_msg void OnBnClickedShader();
};
