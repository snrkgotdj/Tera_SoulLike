#pragma once
#include "Script.h"

enum PLAYER_STATE;
enum class DIR_STATE;
class CGameObject;

enum class CAMERA_STATE
{
	FOLLOW,
	FREE,
	END,
};

class CThirdCamScript :
	public CScript
{
private:
	CGameObject*	m_pTargetObj;
	float			m_fDistance;
	float			m_fHeight;
	float			m_fRight;

	float			m_fWalkSpeed;
	float			m_fRunSpeed;

	CAMERA_STATE	m_eState;

	bool			m_bVibe;
	Vec3			m_vPrevPos;
	float			m_fPower;

public:
	virtual void Awake();
	virtual int Update();

public:
	void SetTargetObj(CGameObject* _pObj) { m_pTargetObj = _pObj; }
	void GetPos(const PLAYER_STATE& _eState, const DIR_STATE& _eDir, Vec3& _vOutPos);
	void GetDir(Vec3& _vPos);
	float GetWalkSpeed() { return m_fWalkSpeed; }
	void SetVibration(bool _bVibe, float _fPower = 0.f, bool _bMust = false);

	void SetState(const CAMERA_STATE& _eState);

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	CLONE(CThirdCamScript)

public:
	CThirdCamScript();
	virtual ~CThirdCamScript();
};

