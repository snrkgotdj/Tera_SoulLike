#pragma once

#include "global.h"

class CRes
{
private:
	static UINT g_iID;

private:
	int		m_iRefCnt;	
	UINT    m_iResID;
	wstring m_strKey;	// ���ҽ� �Ŵ����� ��� �ɶ��� Ű��
	wstring m_strPath;	// �����

public:
	void AddRef() { ++m_iRefCnt; }
	void SubRef() { --m_iRefCnt; }

	virtual void UpdateData() {};
	virtual CRes* Clone() { return NULL; }
	UINT GetResID() { return m_iResID; }
private:
	void SetKey(const wstring& _strKey) { m_strKey = _strKey; };
	void SetPath(const wstring& _strPath) { m_strPath = _strPath; };
	int GetRefCount() { return m_iRefCnt; }
	

public:
	const wstring& GetKey() { return m_strKey; }
	const wstring& GetPath() { return m_strPath; }

public:
	virtual void Save(FILE* _pFile);
	static CRes* Load(FILE* _pFile) {};


public:
	CRes();
	CRes(const CRes& _other);
	virtual ~CRes();

	friend class CResMgr;
};

