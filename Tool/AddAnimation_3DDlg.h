#pragma once
#include "afxwin.h"

class CGameObject;

class CAddAnimation_3DDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddAnimation_3DDlg)

public:
	CAddAnimation_3DDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddAnimation_3DDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDANIMATION_3DDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CGameObject* m_pTargetObj;
	CEdit m_editKey;
	CEdit m_editStart;
	CEdit m_editEnd;

public:
	void SetTarGet(CGameObject* _pTargetObj) { m_pTargetObj = _pTargetObj; }

	virtual void OnOK();
};
