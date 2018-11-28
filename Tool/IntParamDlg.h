#pragma once

#include "ShaderParam.h"
#include "afxwin.h"

// CIntParamDlg 대화 상자입니다.

class CIntParamDlg : public CShaderParam
{
	DECLARE_DYNAMIC(CIntParamDlg)

public:
	CIntParamDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CIntParamDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTPARAMDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_editParam;
	CEdit m_editData;
	bool m_bFocus;

public:
	virtual void Init(CMaterial* _pMtrl, const tShaderParam& _tParam, UINT INum);
	virtual void UpdateParam();
	afx_msg void OnEnChange();
	virtual void PostNcDestroy();
	afx_msg void OnEnSetfocus();
	afx_msg void OnEnKillfocus();
};
