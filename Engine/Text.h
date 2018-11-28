#pragma once
#include "Component.h"

// Font
#pragma comment(lib, "FW1FontWrapper")

class CText :
	public CComponent
{
private:
	IFW1Factory*		m_pFW1Factory;
	IFW1FontWrapper*	m_pFontWrapper;

	wstring				m_strText;
	float				m_fSize;
	UINT				m_iColor;
	UINT				m_arrColorInfo[3];

public:
	virtual void Awake();
	virtual int Update();
	virtual void Render();

public:
	void SetSize(float _fSize) { m_fSize = _fSize; }
	float GetSize() { return m_fSize; }
	void SetColor(UINT _iColor) { m_iColor = _iColor; }
	UINT* GetColorInfo() { return m_arrColorInfo; }
	UINT MakeColor(UINT _arrColorInfo[]);
	void SetText(const wstring& _strText) { m_strText = _strText; }
	const wstring& GetText() { return m_strText; }

private:
	Vec2 GetWindowPosFromWorldPos();

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CLONE(CText)

public:
	CText();
	virtual ~CText();
};

