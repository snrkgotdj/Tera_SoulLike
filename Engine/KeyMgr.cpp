#include "KeyMgr.h"

#include "Core.h"
#include "RenderMgr.h"

CKeyMgr::CKeyMgr()
	: m_vMouse(Vec2(0.f, 0.f))
	, m_vOldMouse(Vec2(0.f, 0.f))
{
}

CKeyMgr::~CKeyMgr()
{
}

void CKeyMgr::init()
{
	for (int i = 0; i < (int)KEY_TYPE::END; ++i)
	{
		m_arrKey[i].eState = KEY_STATE::STATE_NONE;
	}

	m_arrKey[(int)KEY_TYPE::KEY_1].iVK = '1';
	m_arrKey[(int)KEY_TYPE::KEY_2].iVK = '2';
	m_arrKey[(int)KEY_TYPE::KEY_3].iVK = '3';
	m_arrKey[(int)KEY_TYPE::KEY_4].iVK = '4';
	m_arrKey[(int)KEY_TYPE::KEY_5].iVK = '5';
	m_arrKey[(int)KEY_TYPE::KEY_6].iVK = '6';
	m_arrKey[(int)KEY_TYPE::KEY_7].iVK = '7';
	m_arrKey[(int)KEY_TYPE::KEY_8].iVK = '8';
	m_arrKey[(int)KEY_TYPE::KEY_9].iVK = '9';
	m_arrKey[(int)KEY_TYPE::KEY_0].iVK = '0';
	m_arrKey[(int)KEY_TYPE::KEY_Q].iVK = 'Q';
	m_arrKey[(int)KEY_TYPE::KEY_W].iVK = 'W';
	m_arrKey[(int)KEY_TYPE::KEY_E].iVK = 'E';
	m_arrKey[(int)KEY_TYPE::KEY_R].iVK = 'R';
	m_arrKey[(int)KEY_TYPE::KEY_T].iVK = 'T';
	m_arrKey[(int)KEY_TYPE::KEY_Y].iVK = 'Y';
	m_arrKey[(int)KEY_TYPE::KEY_U].iVK = 'U';
	m_arrKey[(int)KEY_TYPE::KEY_I].iVK = 'I';
	m_arrKey[(int)KEY_TYPE::KEY_O].iVK = 'O';
	m_arrKey[(int)KEY_TYPE::KEY_P].iVK = 'P';
	m_arrKey[(int)KEY_TYPE::KEY_A].iVK = 'A';
	m_arrKey[(int)KEY_TYPE::KEY_S].iVK = 'S';
	m_arrKey[(int)KEY_TYPE::KEY_D].iVK = 'D';
	m_arrKey[(int)KEY_TYPE::KEY_F].iVK = 'F';
	m_arrKey[(int)KEY_TYPE::KEY_G].iVK = 'G';
	m_arrKey[(int)KEY_TYPE::KEY_H].iVK = 'H';
	m_arrKey[(int)KEY_TYPE::KEY_J].iVK = 'J';
	m_arrKey[(int)KEY_TYPE::KEY_K].iVK = 'K';
	m_arrKey[(int)KEY_TYPE::KEY_L].iVK = 'L';
	m_arrKey[(int)KEY_TYPE::KEY_Z].iVK = 'Z';
	m_arrKey[(int)KEY_TYPE::KEY_X].iVK = 'X';
	m_arrKey[(int)KEY_TYPE::KEY_C].iVK = 'C';
	m_arrKey[(int)KEY_TYPE::KEY_V].iVK = 'V';
	m_arrKey[(int)KEY_TYPE::KEY_B].iVK = 'B';
	m_arrKey[(int)KEY_TYPE::KEY_N].iVK = 'N';
	m_arrKey[(int)KEY_TYPE::KEY_M].iVK = 'M';
	m_arrKey[(int)KEY_TYPE::KEY_UP].iVK = VK_UP;
	m_arrKey[(int)KEY_TYPE::KEY_DOWN].iVK = VK_DOWN;
	m_arrKey[(int)KEY_TYPE::KEY_LEFT].iVK = VK_LEFT;
	m_arrKey[(int)KEY_TYPE::KEY_RIGHT].iVK = VK_RIGHT;
	m_arrKey[(int)KEY_TYPE::KEY_LSHIFT].iVK = VK_LSHIFT;
	m_arrKey[(int)KEY_TYPE::KEY_RSHIFT].iVK = VK_RSHIFT;
	m_arrKey[(int)KEY_TYPE::KEY_SPACE].iVK = VK_SPACE;
	m_arrKey[(int)KEY_TYPE::KEY_INSERT].iVK = VK_INSERT;
	m_arrKey[(int)KEY_TYPE::KEY_DEL].iVK = VK_DELETE;
	m_arrKey[(int)KEY_TYPE::KEY_HOME].iVK = VK_HOME;
	m_arrKey[(int)KEY_TYPE::KEY_END].iVK = VK_END;
	m_arrKey[(int)KEY_TYPE::KEY_PAGEUP].iVK = VK_PRIOR;
	m_arrKey[(int)KEY_TYPE::KEY_PAGEDOWN].iVK = VK_NEXT;
	m_arrKey[(int)KEY_TYPE::KEY_ESC].iVK = VK_ESCAPE;
	m_arrKey[(int)KEY_TYPE::KEY_F1].iVK = VK_F1;
	m_arrKey[(int)KEY_TYPE::KEY_F2].iVK = VK_F2;
	m_arrKey[(int)KEY_TYPE::KEY_F3].iVK = VK_F3;
	m_arrKey[(int)KEY_TYPE::KEY_F4].iVK = VK_F4;
	m_arrKey[(int)KEY_TYPE::KEY_F5].iVK = VK_F5;
	m_arrKey[(int)KEY_TYPE::KEY_F6].iVK = VK_F6;
	m_arrKey[(int)KEY_TYPE::KEY_F7].iVK = VK_F7;
	m_arrKey[(int)KEY_TYPE::KEY_F8].iVK = VK_F8;
	m_arrKey[(int)KEY_TYPE::KEY_F9].iVK = VK_F9;
	m_arrKey[(int)KEY_TYPE::KEY_F10].iVK = VK_F10;
	m_arrKey[(int)KEY_TYPE::KEY_F11].iVK = VK_F11;
	m_arrKey[(int)KEY_TYPE::KEY_F12].iVK = VK_F12;
	m_arrKey[(int)KEY_TYPE::KEY_NUM0].iVK = VK_NUMPAD0;
	m_arrKey[(int)KEY_TYPE::KEY_NUM1].iVK = VK_NUMPAD1;
	m_arrKey[(int)KEY_TYPE::KEY_NUM2].iVK = VK_NUMPAD2;
	m_arrKey[(int)KEY_TYPE::KEY_NUM3].iVK = VK_NUMPAD3;
	m_arrKey[(int)KEY_TYPE::KEY_NUM4].iVK = VK_NUMPAD4;
	m_arrKey[(int)KEY_TYPE::KEY_NUM5].iVK = VK_NUMPAD5;
	m_arrKey[(int)KEY_TYPE::KEY_NUM6].iVK = VK_NUMPAD6;
	m_arrKey[(int)KEY_TYPE::KEY_NUM7].iVK = VK_NUMPAD7;
	m_arrKey[(int)KEY_TYPE::KEY_NUM8].iVK = VK_NUMPAD8;
	m_arrKey[(int)KEY_TYPE::KEY_NUM9].iVK = VK_NUMPAD9;
	m_arrKey[(int)KEY_TYPE::KEY_ENTER].iVK = VK_RETURN;
	m_arrKey[(int)KEY_TYPE::KEY_CTRL].iVK = VK_CONTROL;
	m_arrKey[(int)KEY_TYPE::KEY_ALT].iVK = VK_MENU;
	m_arrKey[(int)KEY_TYPE::KEY_TAB].iVK = VK_TAB;
	m_arrKey[(int)KEY_TYPE::KEY_CAPS].iVK = VK_CAPITAL;	
	m_arrKey[(int)KEY_TYPE::KEY_LBTN].iVK = VK_LBUTTON;
	m_arrKey[(int)KEY_TYPE::KEY_RBTN].iVK = VK_RBUTTON;
	m_arrKey[(int)KEY_TYPE::KEY_MBTN].iVK = VK_MBUTTON;
}

void CKeyMgr::Update()
{
	for (int i = 0; i < (int)KEY_TYPE::END; ++i)
	{
		if (GetAsyncKeyState(m_arrKey[i].iVK) & 0x8000)
		{			
			if (KEY_STATE::STATE_NONE == m_arrKey[i].eState
				|| KEY_STATE::STATE_AWAY == m_arrKey[i].eState)
				// 이전에는 안눌렸거나 키를 땐 상황, 현재 키가 눌림
				m_arrKey[i].eState = STATE_TAB;
			else
				m_arrKey[i].eState = STATE_HOLD;
		}
		else // 현재 키가 안눌림
		{
			if (KEY_STATE::STATE_NONE == m_arrKey[i].eState)
				continue;

			if (KEY_STATE::STATE_AWAY == m_arrKey[i].eState)
				m_arrKey[i].eState = STATE_NONE;			
			else
				m_arrKey[i].eState = STATE_AWAY;			
		}
	}
	
	// 마우스 좌표 계산
	m_vOldMouse = m_vMouse;

	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	HWND hwnd = CCore::GetInst()->GetMainWndHwnd();
	ScreenToClient(hwnd, &ptMouse);
	m_vMouse.x = (float)ptMouse.x;
	m_vMouse.y = (float)ptMouse.y;
	
	m_vMouseDir.x = m_vMouse.x - m_vOldMouse.x;
	m_vMouseDir.y = m_vOldMouse.y - m_vMouse.y;
}

bool CKeyMgr::GetKeyState(KEY_TYPE _key, KEY_STATE _eState)
{
	if (_eState == m_arrKey[(int)_key].eState)
		return true;
	return false;
}