#pragma once
#include "Component.h"


class CTransform :
	public CComponent
{
private:
	Vec3		m_vPos;
	Vec3		m_vScale;
	Vec3		m_vRot;	

	Vec3		m_vDir[(UINT)DIR_TYPE::END];
	
	Matrix		m_matWorld;
	Matrix		m_matBillBoard;
	Matrix		m_matPrevWVP;

	bool		m_bCull;
	bool		m_bBillBoard;

	float		m_fCulRadius;

	////////////////////////////
	bool m_bIsParts;
	Matrix m_matBone;
	//////////////////////////////                                                                                        

	int			m_iRimNum;

public:
	virtual int Update();
	virtual int LateUpdate();
	virtual int FinalUpdate();

private:
	void UpdateData();
	void UpdateData(Matrix _m_matWorld);

private:
	void RecalculationBeChild();
	void RecalculationBeParent();
	void SetAnimationFinalPos();

public:
	Vec3 GetLocalPos() { return m_vPos; }	
	void SetLocalPos(const Vec3& _vPos) { m_vPos = _vPos; }
	Vec3 GetWorldPos() const;

	Vec3 GetLocalScale() { return m_vScale; }
	void SetLocalScale(const Vec3& _vScale) { m_vScale = _vScale; }
	Vec3 GetWorldScale();

	Vec3 GetLocalRot() { return m_vRot; }
	void SetLocalRot(const Vec3& _vRot) { m_vRot = _vRot; }

	const Matrix& GetWorldMat() { return m_matWorld; }
	const Vec3& GetDir(DIR_TYPE _eType) const { return m_vDir[(UINT)_eType]; }
	float GetCulRadius() { return m_fCulRadius; }

	void SetLocalRotMat(const Matrix& _mat);

	bool IsCull() { return m_bCull; }
	void SetCamCull(bool _bTrue) { m_bCull = _bTrue; }
	void SetBillBoard() { m_bBillBoard = true; }
	void SetBillBoardMatrix(const XMMATRIX& _mat) { m_matBillBoard = _mat; }

public:
	Vec3 DecomposeRotMat(const Matrix& _matRot);
	void SetBoneMatrix(Matrix _mat) { m_bIsParts = true, m_matBone = _mat; }
	void SetRimNum(int iNum) { m_iRimNum = iNum; }

public:
	virtual CTransform* Clone() { return new CTransform(*this); }
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);


public:
	CTransform();
	virtual ~CTransform();

	friend class CMeshRender;
	friend class CGameObject;
	friend class CCamFilter;
	friend class CHDRFilter;
	friend class CCamera;
	friend class CTrail;
};

