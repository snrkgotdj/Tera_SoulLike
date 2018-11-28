#pragma once

#include "global.h"

#include "Component.h"
#include "RenderMgr.h"

template<typename T>
struct ID{typedef T type;};

class CTransform;
class CMeshRender;
class CCamera;
class CScript;
class CCollider;
class CAnimator_2D;
class CAnimator_3D;
class CLight;
class CTerrain;
class CParticle;
class CText;
class CTrail;
class CAudio;

class CGameObject
{
private:
	wstring					m_strTag;
	wstring					m_strLayerName;
	CComponent*				m_arrCom[(UINT)COMPONENT_TYPE::END];
	map<SCRIPT_ID, CScript*> m_mapScript;

	CGameObject*			m_pParentObj;
	list<CGameObject*>		m_listChildObj;

	bool					m_bActive;
	bool					m_bDestroy;

	int					m_iPartsIdx;
	bool					m_bAnimatorActive;

public:
	void Awake();
	void Start();
	int Update();
	int LateUpdate();
	int DelayUpdate();
	int FinalUpdate(); // transform 
	void Render();

public:
	void SetTag(const wstring& _strTag) { m_strTag = _strTag; }
	const wstring& GetTag() { return m_strTag; }
	const wstring& GetLayerName() { return m_strLayerName; }
	void SetActive(bool _bTrue);
	void SetAnimationActive(bool _bTrue) { m_bAnimatorActive = _bTrue; }
	bool IsActive() { return m_bActive; }
	bool IsActiveAnim() { return m_bAnimatorActive; }
	void Destroy();
	bool IsDestroy() { return m_bDestroy; }
	int DeleteScript(int _iIdx);
	int DeleteScript(SCRIPT_ID _eID);
	int DeleteAllScript();
	void ClearScript() { m_mapScript.clear(); }
	void DeleteComponent(COMPONENT_TYPE _eTYPE) { SAFE_DELETE(m_arrCom[(UINT)_eTYPE]); }

	void SetPartsIdx(int _iIdx) { m_iPartsIdx = _iIdx; }
	int GetPartsIdx() { return m_iPartsIdx; }

private:
	void SetLayerName(const wstring& _strLayerName) { m_strLayerName = _strLayerName; }

public:
	CTransform* Transform() { return (CTransform*)m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM]; }
	CMeshRender* MeshRender() { return (CMeshRender*)m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER]; }
	CCollider* Collider() { return (CCollider*)m_arrCom[(UINT)COMPONENT_TYPE::COLLIDER]; }
	CAnimator_2D* Animator_2D() { return (CAnimator_2D*)m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_2D]; }
	CAnimator_3D* Animator_3D() { return (CAnimator_3D*)m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_3D]; }
	CCamera* Camera() { return (CCamera*)m_arrCom[(UINT)COMPONENT_TYPE::CAMERA]; }
	CLight* Light() { return (CLight*)m_arrCom[(UINT)COMPONENT_TYPE::LIGHT]; }
	CTerrain* Terrain() { return (CTerrain*)m_arrCom[(UINT)COMPONENT_TYPE::TERRAIN]; }
	CText* Text() { return (CText*)m_arrCom[(UINT)COMPONENT_TYPE::TEXT]; }
	CParticle* Particle() {return (CParticle*)m_arrCom[(UINT)COMPONENT_TYPE::PARTICLE];	}
	CTrail* Trail() { return (CTrail*)m_arrCom[(UINT)COMPONENT_TYPE::TRAIL]; }
	CAudio* Audio() { return (CAudio*)m_arrCom[(UINT)COMPONENT_TYPE::AUDIO]; }
	
	CGameObject* GetParentObj() { return m_pParentObj; }
	list<CGameObject*>& GetChildList() { return m_listChildObj; }
	map<SCRIPT_ID, CScript*>& GetScriptList() { return m_mapScript; }

	bool IsMouseOn(Vec2 _vWorldPos);

public:
	CComponent* GetComponent(COMPONENT_TYPE _eTYPE) { return m_arrCom[(UINT)_eTYPE]; }

	template<typename T>
	CComponent* GetComponent();

	template<typename T>
	void AddComponent(CComponent* _pCom)
	{
		AddComponent(_pCom, ID<T>());
	}

	void AddComponent(COMPONENT_TYPE _eType);

	CScript* FindScript(SCRIPT_ID _id);
	void AddScript(SCRIPT_ID _id, CScript* _pScript);

private:
	template<typename T>
	void AddComponent(CComponent* _pCom, ID<T> _id);
	void AddComponent(CComponent* _pCom, ID<CScript>);

public:
	int AddChildObj(CGameObject* _pChildObj, bool _bStay = true);
	CGameObject* FindChild(const wstring& _strChildName);
	bool HasParent() { return (m_pParentObj == NULL) ?  false : true;}
	void ClearParent();
	bool IsAncestor(CGameObject* _pObj);
	
public:
	CGameObject* Clone() { return new CGameObject(*this); }

public:
	CGameObject();
	CGameObject(const CGameObject& _pOther);
	~CGameObject();

	friend class CLayer;
	friend class CSaveLoadMgr;
};

template<typename T>
CComponent* CGameObject::GetComponent()
{
	const type_info& info = typeid(T);

	if (info.hash_code() == typeid(CTransform).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM];
	}
	else if (info.hash_code() == typeid(CMeshRender).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER];
	}
	else if (info.hash_code() == typeid(CCamera).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::CAMERA];
	}
	else if (info.hash_code() == typeid(CCollider).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::COLLIDER];
	}
	else if (info.hash_code() == typeid(CAnimator_2D).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_2D];
	}
	else if (info.hash_code() == typeid(CAnimator_3D).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_3D];
	}
	else if (info.hash_code() == typeid(CLight).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::LIGHT];
	}
	else if (info.hash_code() == typeid(CText).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::TEXT];
	}
	else if (info.hash_code() == typeid(CTerrain).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::TERRAIN];
	}
	else if (info.hash_code() == typeid(CParticle).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::PARTICLE];
	}
	else if (info.hash_code() == typeid(CTrail).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::TRAIL];
	}
	else if (info.hash_code() == typeid(CAudio).hash_code())
	{
		return m_arrCom[(UINT)COMPONENT_TYPE::AUDIO];
	}
	else
	{
		map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();
		for (; iter != m_mapScript.end(); ++iter)
		{
			if (dynamic_cast<T*>(iter->second))
			{
				return iter->second;
			}
		}
	}
	return NULL;
}

template<typename T>
inline void CGameObject::AddComponent(CComponent * _pCom, ID<T>)
{
	const type_info& info = typeid(T);

	if (info.hash_code() == typeid(CTransform).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM] = _pCom;
	}
	else if (info.hash_code() == typeid(CMeshRender).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER] = _pCom;
	}
	else if (info.hash_code() == typeid(CCamera).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::CAMERA] = _pCom;
	}	
	else if (info.hash_code() == typeid(CCollider).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::COLLIDER] = _pCom;
	}
	else if (info.hash_code() == typeid(CAnimator_2D).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_2D] = _pCom;
	}
	else if (info.hash_code() == typeid(CAnimator_3D).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_3D] = _pCom;
	}
	else if (info.hash_code() == typeid(CLight).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::LIGHT] = _pCom;

		if (!m_strLayerName.empty())
		{
			CRenderMgr::GetInst()->AddLightObject(this);
		}
	}
	else if (info.hash_code() == typeid(CText).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::TEXT] = _pCom;

		if (!m_strLayerName.empty())
		{
			CRenderMgr::GetInst()->AddTextObject(this);
		}
	}
	else if (info.hash_code() == typeid(CTerrain).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::TERRAIN] = _pCom;
	}
	else if (info.hash_code() == typeid(CParticle).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::PARTICLE] = _pCom;
	}
	else if (info.hash_code() == typeid(CTrail).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::TRAIL] = _pCom;
	}
	else if (info.hash_code() == typeid(CAudio).hash_code())
	{
		m_arrCom[(UINT)COMPONENT_TYPE::AUDIO] = _pCom;
	}
	else
	{
		assert(NULL);
	}

	_pCom->SetGameObject(this);
}