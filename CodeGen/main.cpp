#include <iostream>
#include <Windows.h>
#include <vector>
using namespace std;

#include "Pathmgr.h"

vector<wstring> g_vecName;
vector<wstring> g_vecMgrName;

int main()
{
	CPathMgr::Init();
	wstring strSolutionPath = CPathMgr::GetSolutionPath();
	wstring strScriptMgr = strSolutionPath + L"ScriptLib\\MgrScript\\ScriptMgr.cpp";

	// 1. ���� �����ϴ� ��� ��ũ��Ʈ�� �˾Ƴ�����.
	wstring strScriptCode = strSolutionPath + L"ScriptLib";

	WIN32_FIND_DATA tData = {};
	HANDLE handle = FindFirstFile(wstring(strScriptCode + L"\\*.h").c_str(), &tData);

	if (INVALID_HANDLE_VALUE == handle)
		return 0;

	while (true)
	{
		g_vecName.push_back(wstring(tData.cFileName).substr(0, wcslen(tData.cFileName) - 2));
		if (!FindNextFile(handle, &tData))
			break;
	}

	// 1. MgrScript
	// 1. ���� �����ϴ� ��� ��ũ��Ʈ�� �˾Ƴ�����.
	strScriptCode = strScriptCode + L"\\MgrScript";

	tData = {};
	handle = FindFirstFile(wstring(strScriptCode + L"\\*.h").c_str(), &tData);

	if (INVALID_HANDLE_VALUE == handle)
		return 0;

	while (true)
	{
		wstring strHeader = wstring(tData.cFileName).substr(0, wcslen(tData.cFileName) - 2);
		if (L"STScript" != strHeader && L"ScriptMgr" != strHeader)
			g_vecMgrName.push_back(strHeader);
		if (!FindNextFile(handle, &tData))
			break;
	}

	FindClose(handle);


	FILE* pFile = NULL;

	_wfopen_s(&pFile, strScriptMgr.c_str(), L"w");

	// ��� �Է�
	fwprintf_s(pFile, L"#include \"ScriptMgr.h\"\n\n");

	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		fwprintf_s(pFile, L"#include \"..\\");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L".h\"\n");
	}
	for (UINT i = 0; i < g_vecMgrName.size(); ++i)
	{
		fwprintf_s(pFile, L"#include \"..\\MgrScript\\");
		fwprintf_s(pFile, g_vecMgrName[i].c_str());
		fwprintf_s(pFile, L".h\"\n");
	}

	// ù ��° �Լ� �ۼ�
	fwprintf_s(pFile, L"\nvoid CScriptMgr::GetScriptInfo(vector<tScriptInfo>& _vec)\n{\n");

	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		fwprintf_s(pFile, L"\t_vec.push_back(tScriptInfo{ L\"");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L"\", (DWORD)typeid(C");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L").hash_code() });\n");
	}
	fwprintf_s(pFile, L"}\n\n");


	// �ι�° �Լ� �ۼ�
	fwprintf_s(pFile, L"CScript * CScriptMgr::GetScript(DWORD _dwTypeId)\n{\n");

	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		fwprintf_s(pFile, L"\tif (typeid(C");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L").hash_code() == _dwTypeId)\n");
		fwprintf_s(pFile, L"\t\treturn new C");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L";\n");
	}
	fwprintf_s(pFile, L"\treturn nullptr;\n}\n\n");

	// ����° �Լ� �ۼ�
	fwprintf_s(pFile, L"CScript * CScriptMgr::GetScript(const wstring& _strScriptName)\n{\n");

	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		fwprintf_s(pFile, L"\tif (L\"");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L"\" == _strScriptName)\n");
		fwprintf_s(pFile, L"\t\treturn new C");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L";\n");
	}

	for (UINT i = 0; i < g_vecMgrName.size(); ++i)
	{
		fwprintf_s(pFile, L"\tif (L\"");
		fwprintf_s(pFile, g_vecMgrName[i].c_str());
		fwprintf_s(pFile, L"\" == _strScriptName)\n");
		fwprintf_s(pFile, L"\t\treturn new C");
		fwprintf_s(pFile, g_vecMgrName[i].c_str());
		fwprintf_s(pFile, L";\n");
	}
	fwprintf_s(pFile, L"\treturn nullptr;\n}\n\n");

	// �׹�° �Լ� �ۼ�
	fwprintf_s(pFile, L"const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)\n{\n");

	for (UINT i = 0; i < g_vecName.size(); ++i)
	{
		fwprintf_s(pFile, L"\tif (NULL != dynamic_cast<C");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L"*>(_pScript))\n");
		fwprintf_s(pFile, L"\t\treturn L\"");
		fwprintf_s(pFile, g_vecName[i].c_str());
		fwprintf_s(pFile, L"\";\n");
	}
	for (UINT i = 0; i < g_vecMgrName.size(); ++i)
	{
		fwprintf_s(pFile, L"\tif (NULL != dynamic_cast<C");
		fwprintf_s(pFile, g_vecMgrName[i].c_str());
		fwprintf_s(pFile, L"*>(_pScript))\n");
		fwprintf_s(pFile, L"\t\treturn L\"");
		fwprintf_s(pFile, g_vecMgrName[i].c_str());
		fwprintf_s(pFile, L"\";\n");
	}
	fwprintf_s(pFile, L"\treturn nullptr;\n}\n\n");

	fclose(pFile);

	return 0;
}