#pragma once
#include "afxcmn.h"
#include "afxwin.h"


class CGameObject;

class CAddAnimDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddAnimDlg)

public:
	CAddAnimDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddAnimDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDANIMDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CGameObject* m_pTarget;

private:
	CString m_strFolderPath;

	CListCtrl m_List;
	CString m_strTexName;
	CString m_strAnimName;
	BOOL m_bContinue;
	CEdit m_editTexCount;
	CEdit m_editFrameCountX;
	CEdit m_editFrameCountY;
	CEdit m_editLT;
	CEdit m_editRT;
	CEdit m_editSizeX;
	CEdit m_editSizeY;

	afx_msg void OnBnClicked();
	afx_msg void OnContinueBnClicked();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
public:	
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
};
