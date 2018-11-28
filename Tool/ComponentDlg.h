#pragma once
#include "afxdialogex.h"

class CGameObject;

class CComponentDlg :
	public CDialogEx
{
protected:
	CGameObject*	m_pTargetObj;
	CGameObject*	m_pPrevTargetObj;
	bool			m_bChange;

public:
	virtual void UpdateComponent(CGameObject* _pObj) = 0;	
	void InitTarget() { m_pTargetObj = NULL; }
	void SetChange(bool _bTrue) { m_bChange = _bTrue; }
	void SetPrevTargetObj(CGameObject* _pObj) { m_pPrevTargetObj = _pObj; }

public:
	CComponentDlg();
	CComponentDlg(UINT nIDTemplate, CWnd *pParent = NULL);
	virtual ~CComponentDlg();
	virtual void OnCancel();
	virtual void OnOK();
};

