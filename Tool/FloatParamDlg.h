#pragma once

#include "ShaderParam.h"
#include "afxwin.h"
// CFloatParamDlg ��ȭ �����Դϴ�.

class CFloatParamDlg : public CShaderParam
{
	DECLARE_DYNAMIC(CFloatParamDlg)

public:
	CFloatParamDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFloatParamDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FLOATPARAMDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_editParam;
	CEdit m_editData;
	bool m_bFocus;

public:
	virtual void Init(CMaterial* _pMtrl, const tShaderParam& _tParam, UINT INum);
	virtual void UpdateParam();

	afx_msg void OnEnChangeEdit();
	virtual void PostNcDestroy();
	afx_msg void OnEnSetfocus();
	afx_msg void OnEnKillfocus();
};
