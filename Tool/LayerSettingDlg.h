#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CLayerSettingDlg 대화 상자입니다.

class CLayerSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLayerSettingDlg)

public:
	CLayerSettingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLayerSettingDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAYERSETTINGDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_lcLayer;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDeleteLayer();
	CEdit m_editLayerName;
	afx_msg void OnBnClickedAddLayer();
};
