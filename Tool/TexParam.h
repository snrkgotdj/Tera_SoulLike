#pragma once

#include "ShaderParam.h"
#include "afxwin.h"

// CTexParam ��ȭ �����Դϴ�.

class CTexture;

class CTexParam : public CShaderParam
{
	DECLARE_DYNAMIC(CTexParam)

public:
	CTexParam(CWnd* pParent);   // ǥ�� �������Դϴ�.
	virtual ~CTexParam();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEXPARAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	
private:
	CEdit		m_editParam;
	CEdit		m_editData;
	CStatic		m_Pic;
	CImage		m_Image;
	bool		m_bChange;
	CTexture*	m_pTex;

public:
	virtual void Init(CMaterial* _pMtrl, const tShaderParam& _tParam, UINT INum);
	virtual void UpdateParam() {}
	afx_msg void OnBnClickedButton();
	void CreateImage();
	afx_msg void OnPaint();
	virtual void PostNcDestroy();
};
