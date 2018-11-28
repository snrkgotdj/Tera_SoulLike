#pragma once


// CHierachyDlg dialog

#include "TreeCtrlDlg.h"
#include "afxcmn.h"

class CGameObject;

class CHierachyDlg : public CTreeCtrlDlg
{
	DECLARE_DYNAMIC(CHierachyDlg)

public:
	CHierachyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHierachyDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HIERACHYDLG };
#endif
private:
	HTREEITEM	m_hDragItem;	 // 드래그 중인 아이템
	HTREEITEM	m_hTargetItem; // 목적지

	HTREEITEM	m_hHitEventTarget;	// 마우스 클릭 대상

public:
	void UpdateTreeControl();
	virtual void Init();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

public:
	void AddGameObject(CGameObject* _pObj, HTREEITEM _hParent);
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindragTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnObjectDelete();
	afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAddprefab();
	afx_msg void OnSavegameobj();
};
