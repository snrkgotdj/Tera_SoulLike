#pragma once

#include "Core.h"

#define MM_LOG 5000

class CDebugMgr
{
	SINGLE(CDebugMgr)

public:
	void Log(const wchar_t* _pLog)
	{
		//_cwprintf(_pLog);
		//_cwprintf(L"\n");
#ifdef _TOOL
		SendMessage(CCore::GetInst()->GetMainWndHwnd(), MM_LOG, (WPARAM)_pLog, 0);
#endif
	}
};

