#include "Component.h"
#include "GameObject.h"

CComponent::CComponent()
{
}


CComponent::~CComponent()
{

}

CTransform * CComponent::Transform()
{
	return m_pObj->Transform();
}

CMeshRender * CComponent::MeshRender()
{
	return m_pObj->MeshRender();
}

CCollider * CComponent::Collider()
{
	return m_pObj->Collider();
}

CAnimator_2D * CComponent::Animator_2D()
{
	return m_pObj->Animator_2D();
}

CAnimator_3D * CComponent::Animator_3D()
{
	return m_pObj->Animator_3D();
}

CCamera * CComponent::Camera()
{
	return m_pObj->Camera();
}

CLight * CComponent::Light()
{
	return m_pObj->Light();
}

CTerrain* CComponent::Terrain()
{
	return  m_pObj->Terrain();
}

CText * CComponent::Text()
{
	return m_pObj->Text();
}

CTrail * CComponent::Trail()
{
	return m_pObj->Trail();
}

CAudio * CComponent::Audio()
{
	return m_pObj->Audio();
}

int CComponent::Update()
{
	return 0;
}

CGameObject * CComponent::GetParentObject() const
{
	if (NULL == m_pObj)
		return NULL;

	return m_pObj->GetParentObj();
}