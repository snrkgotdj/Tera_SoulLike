#pragma once
#include "afxwin.h"


// CMaterialDlg 대화 상자입니다.

class CMeshRender;
class CMaterial;
class CShaderParam;

class CMaterialDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMaterialDlg)

public:
	CMaterialDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMaterialDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATERIALDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CMeshRender*				m_pMR;
	CMaterial*					m_pMtrl;
	vector<CShaderParam*>		m_vecParam;
	LONG						m_lStartPosY;
	UINT						m_iIdx;

	CEdit						m_editMtrl;
	CEdit						m_editShader;

	bool						m_bIsOpen;

private:
	void InitShaderParam(const tShaderParam & _tParam);
	void Close();

public:
	void InitMaterial(CMeshRender* _pMR, UINT _iIdx);
	void UpdateParam();

public:
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedMtrl();
	afx_msg void OnBnClickedShader();
	afx_msg void OnBnClickedButton();
	virtual BOOL OnInitDialog();
	CButton m_Button;
};
