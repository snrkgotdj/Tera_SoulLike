#pragma once

#include "global.h"

class CSampler;
class CBlendState;
class CDepthStencilState;
class CSRT;
class CMRT;
class CCamFilter;
class CGameObject;

class CRenderMgr
{

public:
	static RENDER_MODE m_eMode;

	// юс╫ц
public:
	CMRT*								m_arrMRT[(UINT)MRT_TYPE::END];

private:
	map<wstring, CSampler*>				m_mapSampler;
	map<wstring, CBlendState*>			m_mapBlendState;
	map<wstring, CDepthStencilState*>	m_mapDepthStencilState;
	map<wstring, CSRT*>					m_mapRenderTarget;
	map<wstring, CCamFilter*>			m_mapFilter;

	set<CGameObject*>					m_setLightObj;
	list<CGameObject*>					m_listTextObj;

	IDXGISwapChain*						m_pSwapChain;
	HWND								m_hWnd;
	CCamFilter*							m_pLMFilter;
	bool								m_bWindow;
	Vec2								m_vResolution;

	bool								m_bDebugMod;

private:
	void CreateSampler();
	void CreateBlendState();
	void CreateDepthStencilState();

public:
	int Init(HWND _hWnd, bool _bWindow);

	int AddFilter(const wstring& _strKey, CCamFilter* _pFilter);
	int AddSampler(const wstring& _strKey, CSampler* _pSampler);
	int AddBlendState(const wstring& _strKey, CBlendState* _pState);
	int AddDepthStencilState(const wstring& _strKey, CDepthStencilState* _pState);
	int AddRenderTarget(const wstring& _strKey, CSRT* _pTarget);

	CCamFilter* FindFilter(const wstring& _strKey);
	CSampler* FindSampler(const wstring& _strKey);
	CBlendState* FindBlendState(const wstring& _strKey);
	CDepthStencilState* FindDepthStencilState(const wstring& _strKey);
	CSRT* FindRenderTarget(const wstring& _strKey);

	void SetLightMergeFilter(CCamFilter* _pFilter) { m_pLMFilter = _pFilter; }
	Vec2 GetWindowResolution() { return m_vResolution; }

	HWND GetHandle() { return m_hWnd; }

private:
	map<wstring, CCamFilter*>& GetFilterMap() { return m_mapFilter; }

public:
	void Render();

private:
	void RenderPlayMode();
	void RenderMainCam();
	void RenderUICam();
	void RenderToolMode();

	int CreateSwapChain();
	int CreateMRT();

	void UpdateLightBuffer();
	void ClearShaderResource();

private:
	void Reset() { m_setLightObj.clear(); m_listTextObj.clear(); }
	void AddLightObject(CGameObject* _pLightObj);
	void AddTextObject(CGameObject* _pTextObj);
	void DeleteLightObject(CGameObject* _pLightObj) { m_setLightObj.erase(_pLightObj); }
	void DeleteTextObject(CGameObject* _pTextObj);

	friend class CDevice;
	friend class CSaveLoadMgr;
	friend class CScene;
	friend class CLayer;
	friend class CGameObject;
	friend class CListDlg;

private:
	static CRenderMgr* pInst;

public:
	static CRenderMgr* GetInst()
	{
		if (pInst == nullptr)
			pInst = new CRenderMgr;

		return pInst;
	}

	void DistroyInst() { SAFE_DELETE(pInst); }

	CRenderMgr();
	~CRenderMgr();

};