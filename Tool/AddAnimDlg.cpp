// AddAnimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "AddAnimDlg.h"
#include "afxdialogex.h"

#include "PathMgr.h"

#include "GameObject.h"
#include "Animator_2D.h"
#include "Animation_2D.h"


// CAddAnimDlg dialog

IMPLEMENT_DYNAMIC(CAddAnimDlg, CDialogEx)

CAddAnimDlg::CAddAnimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADDANIMDLG, pParent)
	, m_strTexName(_T(""))
	, m_strAnimName(_T(""))
	, m_bContinue(FALSE)
{

}

CAddAnimDlg::~CAddAnimDlg()
{
}

void CAddAnimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT1, m_strTexName);
	DDX_Text(pDX, IDC_EDIT2, m_strAnimName);
	DDX_Check(pDX, IDC_CHECK1, m_bContinue);
	DDX_Control(pDX, IDC_EDIT3, m_editTexCount);
	DDX_Control(pDX, IDC_EDIT8, m_editFrameCountX);
	DDX_Control(pDX, IDC_EDIT4, m_editLT);
	DDX_Control(pDX, IDC_EDIT5, m_editRT);
	DDX_Control(pDX, IDC_EDIT6, m_editSizeX);
	DDX_Control(pDX, IDC_EDIT7, m_editSizeY);
	DDX_Control(pDX, IDC_EDIT13, m_editFrameCountY);
}


BEGIN_MESSAGE_MAP(CAddAnimDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAddAnimDlg::OnBnClicked)
	ON_BN_CLICKED(IDC_CHECK1, &CAddAnimDlg::OnContinueBnClicked)

	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CAddAnimDlg::OnNMClickList)
END_MESSAGE_MAP()


// CAddAnimDlg message handlers
int CALLBACK Proc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) 
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	
	return 0;
}


void CAddAnimDlg::OnBnClicked()
{
	BROWSEINFO br = {};    

    br.lpfn = Proc;

	CString strFolderInitPath = CPathMgr::GetResPath();
	strFolderInitPath += L"Texture\\Animation";
	br.lParam = (LPARAM)strFolderInitPath.GetBuffer();
    br.ulFlags = BIF_USENEWUI;
    br.hwndOwner = m_hWnd;
    br.lpszTitle = L"폴더 선택";
	
    LPITEMIDLIST pidl = NULL;
    if ((pidl = SHBrowseForFolder(&br)) != NULL)
    {
        wchar_t buffer[MAX_PATH];
        if (SHGetPathFromIDList(pidl, buffer))
			m_strFolderPath = buffer;
    }

	WIN32_FIND_DATA tData = {};

	HANDLE hHandle = FindFirstFile(wstring(m_strFolderPath + L"//" + L"*.*").c_str(), &tData);

	m_List.DeleteAllItems();
	int iIdx = 0;

	do {
		CString strEXT = CPathMgr::GetExt(tData.cFileName);

		if (strEXT != L".png" && strEXT != L".jpg" && strEXT != L".jpeg" && strEXT != L".bmp")
			continue;

		m_List.InsertItem(iIdx++, tData.cFileName);

	} while (FindNextFile(hHandle, &tData));
	
}

void CAddAnimDlg::OnContinueBnClicked()
{
	if (m_bContinue)
		m_bContinue = false;
	else
		m_bContinue = true;

	if (m_bContinue)
	{
		// 활성화
		m_editTexCount.SendMessage(EM_SETREADONLY, 0, 0);

		// 비활성화
		m_editLT.SendMessage(EM_SETREADONLY, 1, 0);
		m_editRT.SendMessage(EM_SETREADONLY, 1, 0);
		m_editSizeX.SendMessage(EM_SETREADONLY, 1, 0);
		m_editSizeY.SendMessage(EM_SETREADONLY, 1, 0);
		m_editFrameCountX.SendMessage(EM_SETREADONLY, 1, 0);
		m_editFrameCountY.SendMessage(EM_SETREADONLY, 1, 0);
	}
	else
	{	
		// 비활성화
		m_editTexCount.SendMessage(EM_SETREADONLY, 1, 0);

		// 활성화
		m_editLT.SendMessage(EM_SETREADONLY, 0, 0);
		m_editRT.SendMessage(EM_SETREADONLY, 0, 0);
		m_editSizeX.SendMessage(EM_SETREADONLY, 0, 0);
		m_editSizeY.SendMessage(EM_SETREADONLY, 0, 0);
		m_editFrameCountX.SendMessage(EM_SETREADONLY, 0, 0);
		m_editFrameCountY.SendMessage(EM_SETREADONLY, 1, 0);
	}
}


BOOL CAddAnimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	m_editTexCount.SendMessage(EM_SETREADONLY, 1, 0);

	return TRUE;  
}


void CAddAnimDlg::OnOK()
{
	if (NULL != m_pTarget)
	{
		UpdateData(true);
		
		tDescInfo info = {};
		info.strTexName = m_strTexName;
		info.strAnimName = m_strAnimName;
		info.iContinue = m_bContinue;

		CString str;
		if (m_bContinue)
		{
			m_editTexCount.GetWindowTextW(str);
			info.iFrameCountX = _wtoi(str);
		}
		else
		{
			m_editFrameCountX.GetWindowTextW(str);
			info.iFrameCountX = _wtoi(str);

			m_editFrameCountY.GetWindowTextW(str);
			info.iFrameCountY = _wtoi(str);
			
			m_editLT.GetWindowTextW(str);
			info.vLeftTop.x = (float)_wtof(str);

			m_editRT.GetWindowTextW(str);
			info.vLeftTop.y = (float)_wtof(str);

			m_editSizeX.GetWindowTextW(str);
			info.vSize.x = (float)_wtof(str);

			m_editSizeY.GetWindowTextW(str);
			info.vSize.y = (float)_wtof(str);
		}

		m_pTarget->Animator_2D()->LoadAnimation_2D(&info, m_strFolderPath.GetBuffer());
	}

	CDialogEx::OnOK();
}



void CAddAnimDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int iIdx = m_List.GetSelectionMark();

	if (-1 != iIdx)
	{
		m_strTexName = m_List.GetItemText(iIdx, 0);
		UpdateData(false);
	}

	*pResult = 0;
}
