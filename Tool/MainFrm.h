
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "MySplitterWnd.h"

class CAddGameObjectDlg;
class CGameObject;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
		
private:
	CMySplitterWnd		m_MainSplitter;
	CMySplitterWnd		m_SubSplitter;
	CAddGameObjectDlg*	m_pAddGameObjectDlg;
	bool				m_bOnAddObjDlg;

public:
	HWND GetGameViewHwnd() { return m_SubSplitter.GetPane(1, 0)->m_hWnd; }
	CWnd* GetComponentView() { return m_MainSplitter.GetPane(0, 2); }
	CWnd* GetDebugView() { return m_SubSplitter.GetPane(2, 0); }
	CWnd* GetHierachyView() { return m_MainSplitter.GetPane(0, 1); }
	CWnd* GetBtnView() { return m_SubSplitter.GetPane(0, 0); }
	void Update();
	void SetOnAddObjDlg(bool _bTrue);
	bool IsOnAddObjDlg() { return m_bOnAddObjDlg; }
	CAddGameObjectDlg* GetAddObjDlg() { return m_pAddGameObjectDlg; }

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSceneSave();
	afx_msg void OnSceneLoad();
	afx_msg void OnAddresTexture();
	afx_msg void OnAddresMaterial();
	afx_msg void OnGameobjectCreateterrain();
	afx_msg void OnCreateText();
	afx_msg void OnCreateBotton();
	afx_msg void OnSceneLayersetting();
	afx_msg void OnLoadgameobject();
	afx_msg void OnCreatebaseeffect();
	afx_msg void OnAddgameobjectBasic();
	afx_msg void OnAddgameobjectPrefab();
	afx_msg void OnAddgameobjectWaynode();
};


