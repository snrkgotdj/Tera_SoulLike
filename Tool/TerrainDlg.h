#pragma once

#include "ComponentDlg.h"
#include "afxwin.h"
#include "afxext.h"
#include "afxcmn.h"

// CTerrainDlg 대화 상자입니다.

class CTerrainDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CTerrainDlg)

public:
	CTerrainDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTerrainDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TERRAINDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CButton			m_btnHeightMap;
	CButton			m_btnSplate;
	CButton			m_btnNaviMesh;
	CButton			m_bTess;

	CSliderCtrl		m_sliderControl;
	CEdit			m_editRadius;
	CEdit			m_editPatch;

	CBitmapButton	m_btnBrush1;
	BOOL			m_bBrush1;

	CBitmapButton	m_btnBrush2;
	BOOL			m_bBrush2;

	BOOL			m_arrTile[3];

	CBitmapButton m_btnTile1;
	CBitmapButton m_btnTile2;
	CBitmapButton m_btnTile3;

	bool			m_bIsSplateMod;
	int				m_iYSize;

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnHeightMap();
	afx_msg void OnBnClickedSplat();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBrush1();
	afx_msg void OnBnClickedBrush2();
	afx_msg void OnBnClickedTile(UINT _id); 
	afx_msg void OnBnClickedNaviMesh();
	

	afx_msg void OnBnClickedUseTess();
	
};
