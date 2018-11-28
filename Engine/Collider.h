#pragma once
#include "Component.h"

#include "ResPtr.h"

class CMesh;
class CMaterial;

class CCollider :
	public CComponent
{
	static UINT g_iColID;
protected:
	UINT				m_iColID;
	CResPtr<CMesh>		m_pMesh;
	CResPtr<CMaterial>	m_pMtrl;

	int					m_iCollision;

	Vec3				m_vObjPos;			// �浹ü�� GameObject�� ��ġ
	Vec3				m_vObjScale;		// �浹ü�� GameObject�� ũ��
	Vec3				m_vObjRot;			// �浹ü�� GameObject�� ȸ��
	Vec3				m_vOffsetPos;		// GameObject �κ����� ������� ��ġ
	Vec3				m_vOffsetScale;		// GameObject �κ��� ������� ũ��	

	Matrix				m_matColliderWorld;

	bool				m_bScale;			// GameObject �� ũ�⿡ ������ ������ �ʹ�����.	
	COLLIDER_TYPE		m_eType;

public:
	void SetOffsetPos(const Vec3& _vPos) { m_vOffsetPos = _vPos; }
	void SetOffsetScale(const Vec3& _vScale) { m_vOffsetScale = _vScale; }

	const Vec3& GetOffsetPos() { return m_vOffsetPos; }
	const Vec3& GetOffsetScale() { return m_vOffsetScale; }
	Vec3 GetWorldPos();
	const Vec3& GetWorldScale () { return m_vObjScale; }
	const Vec3& GetLocalRot() { return m_vObjRot; }
	const Matrix& GetWorldMat() { return m_matColliderWorld; }
	
public:
	virtual void OnCollisionEnter(CCollider* _pOther);
	virtual void OnCollision(CCollider* _pOther);
	virtual void OnCollisionExit(CCollider* _pOther);

	virtual bool Is2DCollider() = 0;
	virtual bool Is3DCollider() = 0;
	virtual int FinalUpdate();

	virtual COLLIDER_TYPE GetColType() { return m_eType; }
	virtual void SetColType(const COLLIDER_TYPE& _Type);

	void UpdateData();

public:
	UINT GetColID() { return m_iColID; }
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);


protected:
	CCollider();
	CCollider(const CCollider& _other);
	virtual ~CCollider();
};

