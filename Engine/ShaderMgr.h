#pragma once

#include "global.h"

class CShader;

class CShaderMgr
{
	SINGLE(CShaderMgr);

private:
	map<wstring, CShader*>				m_mapShader;	

public:
	void init();

public:	
	int AddShader(const wstring& _strKey, CShader* _pShader);
	CShader* FindShader(const wstring& _strKey);	
	map<wstring, CShader*>& GetShaderMap() { return m_mapShader; }

private:
	void CreateShader();
};

