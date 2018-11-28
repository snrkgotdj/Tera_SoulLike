#pragma once
#include "afxdialogex.h"


class CTreeCtrlDlg :
	public CDialogEx
{
protected:
	CTreeCtrl	m_ctrlTree;

protected:
	HTREEITEM AddItem(const wstring& _strName, HTREEITEM _hParent, DWORD_PTR _dwData);
	void MoveTreeItem(HTREEITEM _hSrcItem, HTREEITEM _hDestItem);
	void GetChildren(HTREEITEM _hParent, vector<HTREEITEM>& _vecOut);
	void DeleteChildren(HTREEITEM _hParent);

public:
	virtual void Init() = 0;

public:
	CTreeCtrlDlg();
	CTreeCtrlDlg(UINT nIDTemplate, CWnd *pParent = NULL);
	virtual ~CTreeCtrlDlg();
};

