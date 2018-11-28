#include "ResPtr.h"

#include "Mesh.h"
#include "Texture.h"
#include "Prefab.h"
#include "Material.h"
#include "Sound.h"
#include "MeshData.h"

template class CResPtr<CMesh>;
template class CResPtr<CTexture>;
template class CResPtr<CPrefab>;
template class CResPtr<CMaterial>;
template class CResPtr<CSound>;
template class CResPtr<CMeshData>;
template class CResPtr<CRes>;

template<typename T>
CResPtr<T>::CResPtr()
	: m_pTarget(NULL)
{
}

template<typename T>
CResPtr<T>::CResPtr(const CResPtr & _Ptr)
	: m_pTarget(_Ptr.m_pTarget)
{
	if (NULL != m_pTarget)
	{
		m_pTarget->AddRef();
	}
}

template<typename T>
CResPtr<T>::CResPtr(T * _pTarget)
	: m_pTarget(_pTarget)
{	
	if (NULL != m_pTarget)
	{
		m_pTarget->AddRef();
	}
}

template<typename T>
CResPtr<T>::~CResPtr()
{
	if (NULL != m_pTarget)
		m_pTarget->SubRef();
}

template<typename T>
void CResPtr<T>::operator = (const CResPtr& _Ptr)
{
	if (m_pTarget == _Ptr.m_pTarget)
	{
		return;
	}

	if(NULL != m_pTarget)
		m_pTarget->SubRef();

	m_pTarget = _Ptr.m_pTarget;

	if (NULL != m_pTarget)
	{
		m_pTarget->AddRef();
	}
}

template<typename T>
void CResPtr<T>::operator = (T * _pTarget)
{
	if (m_pTarget == _pTarget)
	{
		return;
	}

	if(NULL != m_pTarget)
		m_pTarget->SubRef();

	m_pTarget = _pTarget;

	if (NULL != m_pTarget)
	{
		m_pTarget->AddRef();
	}
}

template<typename T>
void CResPtr<T>::Delete()
{
	delete m_pTarget;
	m_pTarget = NULL;
}
