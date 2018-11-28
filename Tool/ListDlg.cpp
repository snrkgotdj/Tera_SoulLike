// ListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "ListDlg.h"
#include "afxdialogex.h"

#include "RenderMgr.h"
#include "ShaderMgr.h"
#include "ResMgr.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "CamFilter.h"

// CListDlg dialog

IMPLEMENT_DYNAMIC(CListDlg, CDialogEx)

CListDlg::CListDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LISTDLG, pParent)
{

}

CListDlg::~CListDlg()
{
}

void CListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CListDlg, CDialogEx)	
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CListDlg::OnNMDblclkList)
END_MESSAGE_MAP()


// CListDlg message handlers


BOOL CListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (RESOURCE_TYPE::MESH == m_eType)
	{
		map<wstring, CResPtr<CMesh>>& mapMesh = CResMgr::GetInst()->GetMeshMap();
		map<wstring, CResPtr<CMesh>>::iterator iter = mapMesh.begin();

		m_vecText.reserve(mapMesh.size());

		for (int i = 0; iter != mapMesh.end(); ++iter, ++i)
		{
			m_vecText.push_back(iter->first.c_str());
			
			LVITEM tItem = {};
			tItem.iItem = i;
			tItem.pszText = m_vecText[i].GetBuffer();
			CRes* pRes = iter->second;
			tItem.lParam = (LPARAM)pRes;
			tItem.mask = LVIF_TEXT | LVIF_PARAM;

			m_ListCtrl.InsertItem(&tItem);
		}		
	}
	else if (RESOURCE_TYPE::TEXTURE == m_eType)
	{
		map<wstring, CResPtr<CTexture>>& textMtrl = CResMgr::GetInst()->GetTextureMap();
		map<wstring, CResPtr<CTexture>>::iterator iter = textMtrl.begin();

		m_vecText.reserve(textMtrl.size());

		for (int i = 0; iter != textMtrl.end(); ++iter, ++i)
		{
			m_vecText.push_back(iter->first.c_str());

			LVITEM tItem = {};
			tItem.iItem = i;
			tItem.pszText = m_vecText[i].GetBuffer();
			CRes* pRes = iter->second;
			tItem.lParam = (LPARAM)pRes;
			tItem.mask = LVIF_TEXT | LVIF_PARAM;

			m_ListCtrl.InsertItem(&tItem);
		}
	}
	else if (RESOURCE_TYPE::SHADER == m_eType)
	{
		map<wstring, CShader*>& mapShader = CShaderMgr::GetInst()->GetShaderMap();
		map<wstring, CShader*>::iterator iter = mapShader.begin();

		m_vecText.reserve(mapShader.size());

		for (int i = 0; iter != mapShader.end(); ++iter, ++i)
		{
			m_vecText.push_back(iter->first.c_str());

			LVITEM tItem = {};
			tItem.iItem = i;
			tItem.pszText = m_vecText[i].GetBuffer();
			CShader* pShader = iter->second;
			tItem.lParam = (LPARAM)pShader;
			tItem.mask = LVIF_TEXT | LVIF_PARAM;

			m_ListCtrl.InsertItem(&tItem);
		}
	}
	else if (RESOURCE_TYPE::MATERIAL == m_eType)
	{
		map<wstring, CResPtr<CMaterial>>& mapMtrl = CResMgr::GetInst()->GetMtrlMap();
		map<wstring, CResPtr<CMaterial>>::iterator iter = mapMtrl.begin();

		m_vecText.reserve(mapMtrl.size());

		for (int i = 0; iter != mapMtrl.end(); ++iter, ++i)
		{
			m_vecText.push_back(iter->first.c_str());

			LVITEM tItem = {};
			tItem.iItem = i;
			tItem.pszText = m_vecText[i].GetBuffer();
			CRes* pRes = iter->second;
			tItem.lParam = (LPARAM)pRes;
			tItem.mask = LVIF_TEXT | LVIF_PARAM;

			m_ListCtrl.InsertItem(&tItem);
		}
	}
	else if (RESOURCE_TYPE::Filter == m_eType)
	{
		map<wstring, CCamFilter*>& mapFilter = CRenderMgr::GetInst()->GetFilterMap();
		map<wstring, CCamFilter*>::iterator iter = mapFilter.begin();

		m_vecText.reserve(mapFilter.size() + 1);

		int iIdx = 0;
		for (iIdx; iter != mapFilter.end(); ++iter, ++iIdx)
		{
			m_vecText.push_back(iter->first.c_str());

			LVITEM tItem = {};
			tItem.iItem = iIdx;
			tItem.pszText = m_vecText[iIdx].GetBuffer();
			CCamFilter* pFilter = iter->second;
			tItem.lParam = (LPARAM)pFilter;
			tItem.mask = LVIF_TEXT | LVIF_PARAM;

			m_ListCtrl.InsertItem(&tItem);
		}

		m_vecText.push_back(L"None");

		LVITEM tItem = {};
		tItem.iItem = iIdx;
		tItem.pszText = m_vecText[iIdx].GetBuffer();
		tItem.lParam = NULL;
		tItem.mask = LVIF_TEXT | LVIF_PARAM;

		m_ListCtrl.InsertItem(&tItem);

	}

	return TRUE; 				 
}


void CListDlg::OnOK()
{
	int iIdx = m_ListCtrl.GetSelectionMark();

	if (-1 != iIdx)
	{
		LPARAM lp = m_ListCtrl.GetItemData(iIdx);
		EndDialog(lp);
	}		
}


void CListDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	EndDialog(NULL);
}

void CListDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	OnOK();

	*pResult = 0;
}
