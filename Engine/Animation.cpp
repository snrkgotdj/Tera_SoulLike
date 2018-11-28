#include "Animation.h"

CAnimation::CAnimation()
	: m_bActive(false)
	, m_iCurFrame(0)
	, m_fAccTime(0.f)
	, m_bReverse(false)
{
}

CAnimation::~CAnimation()
{
}

void CAnimation::Save(FILE * _pFile)
{
	SaveResourceKey(this, _pFile);
	fwrite(&m_bActive, 1, 1, _pFile);
	fwrite(&m_iCurFrame, sizeof(int), 1, _pFile);
}

void CAnimation::Load(FILE * _pFile)
{
	m_strKey = LoadResourceKey(_pFile);
	fread(&m_bActive, 1, 1, _pFile);
	fread(&m_iCurFrame, sizeof(int), 1, _pFile);
}