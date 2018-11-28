#pragma once

#include "ShaderParam.h"
#include "afxwin.h"

// CVec4ParamDlg ��ȭ �����Դϴ�.

class CVec4ParamDlg : public CShaderParam
{
	DECLARE_DYNAMIC(CVec4ParamDlg)

public:
	CVec4ParamDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CVec4ParamDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VEC4PARAMDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
