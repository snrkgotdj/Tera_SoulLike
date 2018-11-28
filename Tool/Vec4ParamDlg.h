#pragma once

#include "ShaderParam.h"
#include "afxwin.h"

// CVec4ParamDlg 대화 상자입니다.

class CVec4ParamDlg : public CShaderParam
{
	DECLARE_DYNAMIC(CVec4ParamDlg)

public:
	CVec4ParamDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CVec4ParamDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VEC4PARAMDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CEdit   m_arrEdit[4];
	CEdit	m_editParam;
	bool    m_bFocus[4];

public:
	virtual void Init(CMaterial* _pMtrl, const tShaderParam& _tParam, UINT INum);
	virtual void UpdateParam();
	afx_msg void OnEnSetfocus(UINT _id);
	afx_msg void OnEnKillfocus(UINT _id);
	afx_msg void OnEnChange(UINT _id);
	virtual void PostNcDestroy();
};
