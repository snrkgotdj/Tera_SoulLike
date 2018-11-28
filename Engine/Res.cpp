#include "Res.h"

#include "func.h"

UINT CRes::g_iID = 0;

CRes::CRes()
	: m_iRefCnt(0)
	, m_iResID(g_iID++)
{
}

CRes::CRes(const CRes & _other)
	: m_iRefCnt(0)
	, m_iResID(g_iID++)
	, m_strKey(_other.m_strKey)
	, m_strPath(_other.m_strPath)
{
}

CRes::~CRes()
{
}

void CRes::Save(FILE * _pFile)
{	
	SaveWString(m_strKey, _pFile);
	SaveWString(m_strPath, _pFile);
}