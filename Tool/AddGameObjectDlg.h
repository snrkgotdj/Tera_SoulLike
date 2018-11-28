#pragma once
#include "afxwin.h"


// CAddGameObjectDlg 대화 상자입니다.

class CAddGameObjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddGameObjectDlg)

public:
	CAddGameObjectDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddGameObjectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDGAMEOBJECTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CListBox m_listboxPrefab;

public:
	virtual BOOL OnInitDialog();
	void Instantiate(const Vec2& _vWindowPos);
	virtual void OnCancel();
};
