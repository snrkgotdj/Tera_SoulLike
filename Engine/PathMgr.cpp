#include "PathMgr.h"

#include <string>
#include <Windows.h>

wchar_t CPathMgr::m_szSolutionPath[255] = {};
wchar_t CPathMgr::m_szResPath[255] = {};
wchar_t CPathMgr::m_szExternalPath[255] = {};
wchar_t CPathMgr::m_szExt[20] = {};
wchar_t CPathMgr::m_szFileName[50] = {};
wchar_t CPathMgr::m_szRelativePath[255] = {};

using namespace std;

CPathMgr::CPathMgr()
{
}

CPathMgr::~CPathMgr()
{
}

void CPathMgr::Init()
{
	// Resource 폴더까지의 절대 경로 구하기
	GetCurrentDirectory(255, m_szResPath);

	// 상위 경로
	int iLen = (int)wcslen(m_szResPath);
	for (int i = iLen - 1; i > 0; --i)
	{
		if (m_szResPath[i] == L'\\')
		{
			wmemset(m_szResPath + i, 0, iLen - i);
			break;
		}
	}	
	
	wcscpy_s(m_szSolutionPath, 255, m_szResPath);
	wcscpy_s(m_szExternalPath, 255, m_szResPath);
	wcscat_s(m_szSolutionPath, 255, L"\\");
	wcscat_s(m_szResPath, 255, L"\\Bin\\Resources\\");
	wcscat_s(m_szExternalPath, 255, L"\\External\\");
}

wchar_t * CPathMgr::GetRelativePath(const wchar_t * _pFullPath)
{
	wstring str = _pFullPath;
	if (str.empty())
		return L"";

	int iLen = (int)wcslen(m_szResPath);
	str = str.substr(iLen, str.length() - iLen).c_str();
	wcscpy_s(m_szRelativePath, 255, str.c_str());

	return m_szRelativePath;
}

wchar_t * CPathMgr::GetExt(const wchar_t * _pFileName)
{
	_wsplitpath_s(_pFileName, NULL, 0, NULL, 0, NULL, 0, m_szExt, 20);
	return m_szExt;
}

wchar_t * CPathMgr::GetFileName(const wchar_t * _pFileName)
{
	_wsplitpath_s(_pFileName, NULL, 0, NULL, 0, m_szFileName, 50, NULL, 0);
	return m_szFileName;
}
