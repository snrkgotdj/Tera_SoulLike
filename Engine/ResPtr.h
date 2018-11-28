#pragma once

#include "Res.h"

template<typename T>
class CResPtr
{
private:
	T*		m_pTarget;

public:	
	operator T* () { return m_pTarget; }
	T* operator -> () { return m_pTarget; }
	CRes* GetTarget() { return (CRes*)m_pTarget; }

public:
	void operator = (const CResPtr& _Ptr);
	void operator = (T* _pTarget);

	void Delete();

public:
	CResPtr();
	CResPtr(const CResPtr& _Ptr);
	CResPtr(T* _pTarget);
	~CResPtr();
};

