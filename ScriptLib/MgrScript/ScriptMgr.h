#pragma once

#include"Script.h"

struct tScriptInfo
{
	wstring strScriptName;
	DWORD	dwTypeId;
};

class CScriptMgr
{
public:
	static void GetScriptInfo(vector<tScriptInfo>& _vec);
	static CScript* GetScript(DWORD _dwTypdId);
	static CScript* GetScript(const wstring& _strScriptName);
	static const wchar_t* GetScriptName(CScript* _pScript);


private:
	CScriptMgr() {};
	~CScriptMgr() {};
};

