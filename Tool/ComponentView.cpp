// ComponentView.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "ComponentView.h"

#include "TransformDlg.h"
#include "MeshRenderDlg.h"
#include "ColliderDlg.h"
#include "CameraDlg.h"
#include "AnimatorDlg.h"
#include "Animator_3DDlg.h"
#include "ScriptDlg.h"
#include "ObjectInfoDlg.h"
#include "LightDlg.h"
#include "TerrainDlg.h"
#include "TextDlg.h"
#include "TrailDlg.h"

#include "GameObject.h"


// CComponentView

IMPLEMENT_DYNCREATE(CComponentView, CFormView)

CComponentView::CComponentView()
	: CFormView(IDD_COMPONENTVIEW)
	, m_arrDlg{}
	, m_pTargetObj(NULL)
	, m_pObjInfoDlg(NULL)
	, m_iStartY(0)
{

}

CComponentView::~CComponentView()
{	
}

void CComponentView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_bnAC);
}

BEGIN_MESSAGE_MAP(CComponentView, CFormView)
	ON_WM_CREATE()
	ON_WM_DESTROY()	
	ON_BN_CLICKED(IDC_BUTTON1, &CComponentView::OnAddComBnClicked)
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CComponentView diagnostics

#ifdef _DEBUG
void CComponentView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CComponentView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CComponentView message handlers
int CComponentView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_arrDlg[(UINT)COMPONENT_TYPE::TRANSFORM] = new CTransformDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::TRANSFORM]->Create(IDD_TRANSFORMDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::TRANSFORM]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::TRANSFORM]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::MESHRENDER] = new CMeshRenderDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::MESHRENDER]->Create(IDD_MESHRENDERDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::MESHRENDER]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::MESHRENDER]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::COLLIDER] = new CColliderDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::COLLIDER]->Create(IDD_COLLIDERDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::COLLIDER]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::COLLIDER]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::CAMERA] = new CCameraDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::CAMERA]->Create(IDD_CAMERADLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::CAMERA]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::CAMERA]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_2D] = new CAnimatorDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_2D]->Create(IDD_ANIMATORDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_2D]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_2D]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_3D] = new CAnimator_3DDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_3D]->Create(IDD_ANIMATOR_3DDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_3D]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::ANIMATOR_3D]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::LIGHT] = new CLightDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::LIGHT]->Create(IDD_LIGHTDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::LIGHT]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::LIGHT]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::TERRAIN] = new CTerrainDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::TERRAIN]->Create(IDD_TERRAINDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::TERRAIN]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::TERRAIN]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::TEXT] = new CTextDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::TEXT]->Create(IDD_TEXTDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::TEXT]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::TEXT]->UpdateWindow();

	m_arrDlg[(UINT)COMPONENT_TYPE::TRAIL] = new CTrailDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::TRAIL]->Create(IDD_TRAILDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::TRAIL]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::TRAIL]->UpdateWindow();
	
	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT] = new CScriptDlg;
	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->Create(IDD_SCRIPTDLG, this);
	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->ShowWindow(false);
	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->UpdateWindow();

	m_pObjInfoDlg = new CObjectInfoDlg;
	m_pObjInfoDlg->Create(IDD_OBJECTINFODLG, this);
	m_pObjInfoDlg->ShowWindow(false);
	m_pObjInfoDlg->UpdateWindow();

	return 0;
}

void CComponentView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if(NULL != m_arrDlg[i])
			m_arrDlg[i]->DestroyWindow();
	}
}

void CComponentView::InitTarget()
{
	m_pTargetObj = NULL;
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL != m_arrDlg[i])
			m_arrDlg[i]->InitTarget();
	}

	m_iStartY = 0;
}

void CComponentView::Init(CGameObject * _pObj)
{
	m_pObjInfoDlg->SetPrevTargetObj(m_pTargetObj);

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL == m_arrDlg[i])
			continue;

		if (NULL != m_pTargetObj && NULL == m_pTargetObj->GetComponent((COMPONENT_TYPE)i))
		{
			if (NULL != m_arrDlg[i])
				continue;
		}

		m_arrDlg[i]->SetPrevTargetObj(m_pTargetObj);
	}

	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->SetPrevTargetObj(m_pTargetObj);

	m_pTargetObj = _pObj;

	m_pObjInfoDlg->SetChange(true);

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL == m_arrDlg[i])
			continue;

		if (NULL == m_pTargetObj->GetComponent((COMPONENT_TYPE)i))
		{
			if (NULL != m_arrDlg[i])
				continue;
		}

		m_arrDlg[i]->SetChange(true);
	}

	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->SetChange(true);

	m_iStartY = 0;
}

void CComponentView::UpdateComponent()
{
	if (NULL == m_pTargetObj)
	{
		m_bnAC.ShowWindow(false);
		return;
	}

	if (m_pTargetObj->IsDestroy())
	{
		InitTarget();
		return;
	}

	m_bnAC.ShowWindow(true);

	INT iStartY = m_iStartY;

	RECT rt = {};
	RECT rtComView = {};
	GetWindowRect(&rtComView);

	m_pObjInfoDlg->GetWindowRect(&rt);
	m_pObjInfoDlg->ShowWindow(true);
	m_pObjInfoDlg->SetWindowPos(NULL, 1, iStartY, rtComView.right - rtComView.left - 2, rt.bottom - rt.top, 0);

	m_pObjInfoDlg->UpdateComponent(m_pTargetObj);

	iStartY += rt.bottom - rt.top - 1;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::SCRIPT; ++i)
	{		
		if (NULL == m_arrDlg[i])
			continue;

		if(NULL == m_pTargetObj->GetComponent((COMPONENT_TYPE)i))
		{
			if (NULL != m_arrDlg[i])
				m_arrDlg[i]->ShowWindow(false);
			continue; 
		}			

		m_arrDlg[i]->GetWindowRect(&rt);
		m_arrDlg[i]->ShowWindow(true);
		m_arrDlg[i]->SetWindowPos(NULL, 1, iStartY, rtComView.right - rtComView.left - 2, rt.bottom - rt.top, 0);
		
		m_arrDlg[i]->UpdateComponent(m_pTargetObj);

		iStartY += rt.bottom - rt.top - 1;
	}

	// Script Dlg
	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->GetWindowRect(&rt);
	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->ShowWindow(true);
	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->SetWindowPos(NULL, 1, iStartY, rtComView.right - rtComView.left - 2, rt.bottom - rt.top, 0);

	m_arrDlg[(UINT)COMPONENT_TYPE::SCRIPT]->UpdateComponent(m_pTargetObj);

	iStartY += rt.bottom - rt.top;
	
	// Add Component Button	
	m_bnAC.GetWindowRect(&rt);
	ScreenToClient(&rt);

	m_bnAC.SetWindowPos(NULL, rt.left, iStartY + 35, rt.right - rt.left, rt.bottom - rt.top, 0);
}

void CComponentView::Update()
{
	UpdateComponent();
}

#include "ComListDlg.h"


void CComponentView::OnAddComBnClicked()
{
	if (NULL == m_pTargetObj)
	{
		return;
	}

	CComListDlg dlg;
	COMPONENT_TYPE type = (COMPONENT_TYPE)dlg.DoModal();

	if (-1 == (UINT)type)
		return;

	if (COMPONENT_TYPE::COLLIDER_2D == type || COMPONENT_TYPE::COLLIDER_3D == type)
	{
		if (NULL != m_pTargetObj->GetComponent(COMPONENT_TYPE::COLLIDER))
			return;

		m_arrDlg[(UINT)COMPONENT_TYPE::COLLIDER]->SetChange(true);
	}
	else
	{
		if (NULL != m_pTargetObj->GetComponent(type))
			return;

		m_arrDlg[(UINT)type]->SetChange(true);
	}

	m_pTargetObj->AddComponent(type);
}


void CComponentView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CSize scrollSize(0, 0);
	SetScrollSizes(MM_TEXT, scrollSize);

}

BOOL CComponentView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (zDelta > 0)
	{
		// 위
		m_iStartY += 10;
	}
	else
	{
		// 아래
		m_iStartY -= 10;
	}

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}
