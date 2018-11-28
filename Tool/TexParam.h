#pragma once

#include "ShaderParam.h"
#include "afxwin.h"

// CTexParam 대화 상자입니다.

class CTexture;

class CTexParam : public CShaderParam
{
	DECLARE_DYNAMIC(CTexParam)

public:
	CTexParam(CWnd* pParent);   // 표준 생성자입니다.
	virtual ~CTexParam();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEXPARAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
