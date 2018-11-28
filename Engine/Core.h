#pragma once

#include "global.h"

class CCore
{
	SINGLE(CCore);
private:
	HWND		m_hWnd;
	
public:
	int init(HWND _hWnd, bool _bWindow);
	int run();
	HWND GetMainWndHwnd() { return m_hWnd; }

private:
	void CreateConstBuffer();

private:
	int Update();
	void Render();
};

