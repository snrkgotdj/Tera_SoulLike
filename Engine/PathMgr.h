#pragma once


class CPathMgr
{
private:
	static wchar_t m_szSolutionPath[255];
	static wchar_t m_szResPath[255];
	static wchar_t m_szExternalPath[255];
	static wchar_t m_szExt[20];
	static wchar_t m_szFileName[50];
	static wchar_t m_szRelativePath[255];
	
private:
	CPathMgr();
	~CPathMgr();

public:
	static void Init();

public:	
	static wchar_t* GetSolutionPath() { return m_szSolutionPath; }
	static wchar_t* GetResPath() { return m_szResPath; }
	static wchar_t* GetRelativePath(const wchar_t* _pFullPath);
	static wchar_t* GetExternalPath() { return m_szExternalPath; }
	static wchar_t* GetExt(const wchar_t* _pFileName);
	static wchar_t* GetFileName(const wchar_t * _pFileName);
};

