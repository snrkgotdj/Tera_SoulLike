#pragma once

#include "ComponentDlg.h"
#include "afxwin.h"

// CMeshRenderDlg 대화 상자입니다.

class CMesh;
class CMaterialDlg;

class CMeshRenderDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CMeshRenderDlg)

public:
	CMeshRenderDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMeshRenderDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESHRENDERDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CEdit					m_editMesh;
	CComboBox				m_cbRS;
	CMesh*					m_pPrevMesh;
	LONG					m_lStartPosY;
	vector<CMaterialDlg*>	m_vecMtrlDlg;


public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	void InitMaterial();

	afx_msg void OnBnClickedMesh();
	afx_msg void OnCbnSelchangeRS();
	BOOL m_bShadowCast;
	BOOL m_bPostEffect;
	afx_msg void OnBnClickedShadow();
	afx_msg void OnBnClickedPostEffect();
	afx_msg void OnBnClickedDelete();
};
