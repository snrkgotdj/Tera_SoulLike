#pragma once
#include "afxext.h"
class CMySplitterWnd :
	public CSplitterWnd
{
private:
	CPoint  m_ptBorder;
	CPoint  m_ptGap;

public:
	void SetBorderSize(CPoint _ptBorder, CPoint _ptGap);

public:
	CMySplitterWnd();
	virtual ~CMySplitterWnd();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

