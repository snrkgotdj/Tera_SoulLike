#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CLayerSettingDlg ��ȭ �����Դϴ�.

class CLayerSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLayerSettingDlg)

public:
	CLayerSettingDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLayerSettingDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAYERSETTINGDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_lcLayer;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDeleteLayer();
	CEdit m_editLayerName;
	afx_msg void OnBnClickedAddLayer();
};
