#pragma once


enum class RES_TYPE
{
	MESH,
	TEXTURE,
	MATERIAL,
	PREFAB,
	CLONE,
	END,
};

// CResDlg dialog
#include "TreeCtrlDlg.h"
class CResDlg : public CTreeCtrlDlg
{
	DECLARE_DYNAMIC(CResDlg)

public:
	CResDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESDLG };
#endif
private:
	HTREEITEM m_arrResRoot[(UINT)RES_TYPE::END];

private:
	HTREEITEM m_hHitPrefab;	// 마우스 클릭 대상

public:
	void UpdateTreeControl();
	virtual void Init();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPrefabInstantiate();
	afx_msg void OnPrefabDelete();
};
