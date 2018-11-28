#include "GameObject.h"

#include "RenderMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "Component.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Animator_2D.h"
#include "Animator_3D.h"
#include "Script.h"
#include "Light.h"
#include "Text.h"
#include "Terrain.h"
#include "Particle.h"
#include "Trail.h"
#include "Audio.h"

#include "Collider_2D.h"
#include "Collider_3D.h"


CGameObject::CGameObject()
	: m_arrCom{}
	, m_pParentObj(NULL)
	, m_bActive(true)
	, m_bDestroy(false)
	, m_iPartsIdx(-1)
	, m_bAnimatorActive(true)
{
}

CGameObject::CGameObject(const CGameObject & _pOther)
	: m_strTag(_pOther.m_strTag)
	, m_arrCom{}
	, m_bActive(_pOther.m_bActive)
	, m_bDestroy(_pOther.m_bDestroy)
	, m_iPartsIdx(_pOther.m_iPartsIdx)
	, m_bAnimatorActive(_pOther.m_bAnimatorActive)
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL != _pOther.m_arrCom[i])
		{
			m_arrCom[i] = _pOther.m_arrCom[i]->Clone();
			m_arrCom[i]->SetGameObject(this);
		}
	}

	map<SCRIPT_ID, CScript*>::const_iterator iter = _pOther.m_mapScript.begin();
	for (; iter != _pOther.m_mapScript.end(); ++iter)
	{
		CScript* pCloneScript = iter->second->Clone();
		pCloneScript->SetGameObject(this);
		m_mapScript.insert(make_pair(iter->first, pCloneScript));
	}

	list<CGameObject*>::const_iterator iterObj = _pOther.m_listChildObj.begin();
	for (; iterObj != _pOther.m_listChildObj.end(); ++iterObj)	
	{
		AddChildObj((*iterObj)->Clone());
	}
}

CGameObject::~CGameObject()
{
	if (Light())
		CRenderMgr::GetInst()->DeleteLightObject(this);
	else if (Text())
		CRenderMgr::GetInst()->DeleteTextObject(this);

	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		SAFE_DELETE(m_arrCom[i]);
	}

	Safe_Delete_Map(m_mapScript);

}

void CGameObject::Awake()
{
	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if(NULL != m_arrCom[i])
			m_arrCom[i]->Awake();
	}
	

	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();
	for (; iter != m_mapScript.end(); ++iter)
	{
		iter->second->Awake();
	}

	list<CGameObject*>::iterator iterChild = m_listChildObj.begin();
	for (; iterChild != m_listChildObj.end(); ++iterChild)
	{
		(*iterChild)->Awake();
	}
}

void CGameObject::Start()
{
	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL != m_arrCom[i])
			m_arrCom[i]->Start();
	}

	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();
	for (; iter != m_mapScript.end(); ++iter)
	{
		iter->second->Start();
	}

	list<CGameObject*>::iterator iterChild = m_listChildObj.begin();
	for (; iterChild != m_listChildObj.end(); ++iterChild)
	{
		(*iterChild)->Start();
	}
}

int CGameObject::Update()
{
	if (!m_bActive)
		return RET_FAILED;

	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL != m_arrCom[i])
			m_arrCom[i]->Update();
	}

	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();
	for (; iter != m_mapScript.end(); ++iter)
	{
		iter->second->Update();
	}

	list<CGameObject*>::iterator iterChild = m_listChildObj.begin();
	for (; iterChild != m_listChildObj.end(); ++iterChild)
	{
		(*iterChild)->Update();
	}

	return 0;
}

int CGameObject::LateUpdate()
{
	if (!m_bActive)
		return RET_FAILED;

	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL != m_arrCom[i])
			m_arrCom[i]->LateUpdate();
	}

	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();
	for (; iter != m_mapScript.end(); ++iter)
	{
		iter->second->LateUpdate();
	}

	list<CGameObject*>::iterator iterChild = m_listChildObj.begin();
	for (; iterChild != m_listChildObj.end(); ++iterChild)
	{
		(*iterChild)->LateUpdate();
	}

	return 0;
}

int CGameObject::DelayUpdate()
{
	if (!m_bActive)
		return RET_FAILED;

	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL != m_arrCom[i])
			m_arrCom[i]->DelayUpdate();
	}

	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();
	for (; iter != m_mapScript.end(); ++iter)
	{
		iter->second->DelayUpdate();
	}

	list<CGameObject*>::iterator iterChild = m_listChildObj.begin();
	for (; iterChild != m_listChildObj.end(); ++iterChild)
	{
		(*iterChild)->DelayUpdate();
	}

	return 0;
}

int CGameObject::FinalUpdate()
{
	if (!m_bActive || m_bDestroy)
		return RET_FAILED;

	for (int i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (NULL != m_arrCom[i])
			m_arrCom[i]->FinalUpdate();
	}

	list<CGameObject*>::iterator iterChild = m_listChildObj.begin();
	for (; iterChild != m_listChildObj.end();)
	{
		if ((*iterChild)->IsDestroy())
		{
			CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
			pScene->FindLayer((*iterChild)->GetLayerName())->DestroyObject(*iterChild);
			iterChild = m_listChildObj.erase(iterChild);
		}
		else
		{
			(*iterChild)->FinalUpdate();
			++iterChild;
		}
	}

	return 0;
}

void CGameObject::Render()
{
	if (!m_bActive)
		return;

	if(NULL != MeshRender())
		MeshRender()->Render();

	if (NULL != Text())
		Text()->Render();
}

void CGameObject::SetActive(bool _bTrue)
{
	m_bActive = _bTrue;
	
	list<CGameObject*>::iterator iter = m_listChildObj.begin();
	for (; iter != m_listChildObj.end(); ++iter)
	{
		(*iter)->SetActive(_bTrue);
	}
}


void CGameObject::Destroy()
{
	m_bDestroy = true;
}

int CGameObject::DeleteScript(int _iIdx)
{
	int iIdx = 0;

	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();
	for (; iter != m_mapScript.end(); ++iter)
	{
		if (_iIdx == iIdx++)
		{
			delete iter->second;
			m_mapScript.erase(iter);
			return RET_SUCCESS;
		}
	}

	return RET_FAILED;
}

int CGameObject::DeleteScript(SCRIPT_ID _iIdx)
{
	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.find(_iIdx);
	
	if (iter != m_mapScript.end())
	{
		delete iter->second;
		m_mapScript.erase(iter);
	}

	return RET_FAILED;
}

int CGameObject::DeleteAllScript()
{
	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.begin();

	for (; iter != m_mapScript.end(); ++iter)
	{
		delete iter->second;
	}

	m_mapScript.clear();

	return RET_FAILED;
}

bool CGameObject::IsMouseOn(Vec2 _vWorldPos)
{
	Vec3 vWorldPos = Transform()->GetWorldPos();
	Vec3 vWorldScale = Transform()->GetWorldScale();

	if (vWorldPos.x - vWorldScale.x / 2.f <= _vWorldPos.x &&  _vWorldPos.x <= vWorldPos.x + vWorldScale.x / 2.f
		&& vWorldPos.y - vWorldScale.y / 2.f <= _vWorldPos.y &&  _vWorldPos.y <= vWorldPos.y + vWorldScale.y / 2.f)
	{
		return true;
	}

	return false;
}

void CGameObject::AddComponent(COMPONENT_TYPE _eType)
{
	switch (_eType)
	{
	case COMPONENT_TYPE::TRANSFORM:
		m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM] = new CTransform;		
		break;
	case COMPONENT_TYPE::MESHRENDER:
		m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER] = new CMeshRender;
		break;
	case COMPONENT_TYPE::COLLIDER_2D:
		m_arrCom[(UINT)COMPONENT_TYPE::COLLIDER] = new CCollider_2D;
		break;
	case COMPONENT_TYPE::COLLIDER_3D:
		m_arrCom[(UINT)COMPONENT_TYPE::COLLIDER] = new CCollider_3D;
		break;
	case COMPONENT_TYPE::ANIMATOR_2D:
		m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_2D] = new CAnimator_2D;
		break;
	case COMPONENT_TYPE::ANIMATOR_3D:
		m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR_3D] = new CAnimator_3D;
		break;
	case COMPONENT_TYPE::CAMERA:
		m_arrCom[(UINT)COMPONENT_TYPE::CAMERA] = new CCamera;
		break;	
	case COMPONENT_TYPE::LIGHT:
		m_arrCom[(UINT)COMPONENT_TYPE::LIGHT] = new CLight;
		break;
	case COMPONENT_TYPE::TERRAIN:
		m_arrCom[(UINT)COMPONENT_TYPE::TERRAIN] = new CTerrain;
		break;
	case COMPONENT_TYPE::TEXT:
		m_arrCom[(UINT)COMPONENT_TYPE::TEXT] = new CText;
		if (!m_strLayerName.empty())
		{
			CRenderMgr::GetInst()->AddTextObject(this);
		}
		break;
	case COMPONENT_TYPE::SCRIPT:
		break;
	case COMPONENT_TYPE::PARTICLE:
		m_arrCom[(UINT)COMPONENT_TYPE::PARTICLE] = new CParticle;
		break;
	case COMPONENT_TYPE::TRAIL:
		m_arrCom[(UINT)COMPONENT_TYPE::TRAIL] = new CTrail;
		break;
	case COMPONENT_TYPE::AUDIO:
		m_arrCom[(UINT)COMPONENT_TYPE::AUDIO] = new CAudio;
		break;
	case COMPONENT_TYPE::END:
		break;
	default:
		assert(NULL);
		break;
	}

	if (COMPONENT_TYPE::COLLIDER_2D == _eType || COMPONENT_TYPE::COLLIDER_3D == _eType)
		_eType = COMPONENT_TYPE::COLLIDER;

	m_arrCom[(UINT)_eType]->SetGameObject(this);
}

void CGameObject::AddComponent(CComponent* _pCom, ID<CScript>)
{
	SCRIPT_ID id = ((CScript*)_pCom)->GetScriptID();
	m_mapScript.insert(make_pair(id, (CScript*)_pCom));
	_pCom->SetGameObject(this);
}

CScript* CGameObject::FindScript(SCRIPT_ID _id)
{
	map<SCRIPT_ID, CScript*>::iterator iter = m_mapScript.find(_id);

	if (iter == m_mapScript.end())
		return nullptr;

	return iter->second;
}

void CGameObject::AddScript(SCRIPT_ID _id, CScript* _pScript)
{
	CScript* pScript = FindScript(_id);

	if (nullptr != pScript)
	{
		MessageBox(nullptr, L"추가하려는 스크립트가 이미 존재합니다", L"스크립트 추가 오류", MB_OK);
		return;
	}

	_pScript->SetScriptID(_id);
	_pScript->SetGameObject(this);
	m_mapScript.insert(make_pair(_id, _pScript));
}

int CGameObject::AddChildObj(CGameObject * _pChildObj, bool _bStay)
{
	// 순환체크
	if (IsAncestor(_pChildObj))
		return RET_FAILED;

	_pChildObj->ClearParent();
	_pChildObj->m_pParentObj = this;
	m_listChildObj.push_back(_pChildObj);

	if (_bStay)
	{
		Transform()->FinalUpdate();
		_pChildObj->Transform()->FinalUpdate();
		_pChildObj->Transform()->RecalculationBeChild();
	}

	return RET_SUCCESS;
}

CGameObject* CGameObject::FindChild(const wstring& _strChildName)
{
	for (auto& Obj : m_listChildObj)
	{
		if (Obj->GetTag() == _strChildName)
		{
			return Obj;
		}
	}

	return nullptr;
}


void CGameObject::ClearParent()
{
	if (NULL == m_pParentObj)
	{
		CLayer* pCurLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(m_strLayerName);
		if (NULL != pCurLayer)
		{
			pCurLayer->RemoveParentObj(this);
		}
		return;
	}

	Transform()->RecalculationBeParent();

	list<CGameObject*>& listChildObj = m_pParentObj->m_listChildObj;

	list<CGameObject*>::iterator iter = listChildObj.begin();
	for (; iter != listChildObj.end(); ++iter)
	{
		if ((*iter) == this)
		{
			listChildObj.erase(iter);
			break;
		}
	}
	
	m_pParentObj = NULL;
}

bool CGameObject::IsAncestor(CGameObject * _pObj)
{
	CGameObject* pParentObj = this;
	list<CGameObject*> listParents;

	while (pParentObj)
	{
		listParents.push_back(pParentObj);
		pParentObj = pParentObj->m_pParentObj;
	}

	list<CGameObject*>::iterator iter = listParents.begin();
	for (iter; iter != listParents.end(); ++iter)
	{
		if ((*iter) == _pObj)
			return true;
	}

	return false;
}
