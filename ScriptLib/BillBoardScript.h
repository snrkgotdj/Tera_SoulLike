#pragma once
#include "Script.h"
class CBillBoardScript :
	public CScript
{
private:
	// 툴에서 확인용 릴리즈할때에는 삭제
#ifdef _DEBUG
	bool m_bOnce;
#endif // DEBUG

private:
	bool	m_bAll;

public:
	virtual void Start();
	virtual int Update();
	virtual int LateUpdate();
	virtual int DelayUpdate();

public:
	void SetYType() { m_bAll = false; }

public:
	CLONE(CBillBoardScript);


public:
	CBillBoardScript();
	virtual ~CBillBoardScript();
};

