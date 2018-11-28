#pragma once
#include "afxwin.h"


// CMaterialDlg ��ȭ �����Դϴ�.

class CMeshRender;
class CMaterial;
class CShaderParam;

class CMaterialDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMaterialDlg)

public:
	CMaterialDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMaterialDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATERIALDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
