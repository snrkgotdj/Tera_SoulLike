#pragma once
#include "afxwin.h"


// CAddGameObjectDlg ��ȭ �����Դϴ�.

class CAddGameObjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddGameObjectDlg)

public:
	CAddGameObjectDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAddGameObjectDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDGAMEOBJECTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CListBox m_listboxPrefab;

public:
	virtual BOOL OnInitDialog();
	void Instantiate(const Vec2& _vWindowPos);
	virtual void OnCancel();
};
