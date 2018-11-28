#pragma once

#include "global.h"

// Vec3
struct Vec3 : XMFLOAT3
{
	Vec3()
		: XMFLOAT3(0.f, 0.f, 0.f)
	{}

	Vec3(FXMVECTOR _xmvec)
		: XMFLOAT3(0.f, 0.f, 0.f)
	{
		XMStoreFloat3(this, _xmvec);
	}

	Vec3(float _x, float _y, float _z)
		: XMFLOAT3(_x, _y, _z)
	{}

	XMVECTOR Convert()
	{
		return XMLoadFloat3((XMFLOAT3*)this);
	}

	XMVECTOR Convert() const
	{
		return XMLoadFloat3((XMFLOAT3*)this);
	}
	Vec4 GetVec4() const
	{
		return Vec4(x, y, z, 0);
	}

	float Distance()
	{
		return sqrt(x * x + y * y + z * z);
	}

	void Normalize()
	{
		float fDist = Distance();

		if (fDist == 0.f)
			assert(NULL);

		x /= fDist;
		y /= fDist;
		z /= fDist;
	}

	float Dot(const Vec3& _vOther)
	{
		return (x * _vOther.x) + (y * _vOther.y) + (z * _vOther.z);
	}

	Vec3 Cross(const Vec3& _vOther)
	{
		return Vec3(y*_vOther.z - z*_vOther.y, z*_vOther.x - x*_vOther.z, x*_vOther.y - y*_vOther.x);
	}

	const Vec3& operator = (FXMVECTOR _xmvec)
	{
		XMStoreFloat3(this, _xmvec);
		return (*this);
	}

	bool operator != (const Vec3& _Vec) const
	{
		if (x == _Vec.x && y == _Vec.y && z == _Vec.z)
			return false;

		return true;
	}


	float& operator[](int _iIdx)
	{
		switch (_iIdx)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			assert(NULL);
			return x;
		}
	}

	
	// 사칙연산
	const Vec3 operator + (FXMVECTOR _other)
	{
		Vec3 vRet(x + _other.vector4_f32[0], y + _other.vector4_f32[1], z + _other.vector4_f32[2]);
		return vRet;
	}

	const Vec3 operator - (FXMVECTOR _other)
	{
		Vec3 vRet(x - _other.vector4_f32[0], y - _other.vector4_f32[1], z - _other.vector4_f32[2]);
		return vRet;
	}

	const Vec3 operator * (FXMVECTOR _other)
	{
		Vec3 vRet(x * _other.vector4_f32[0], y * _other.vector4_f32[1], z * _other.vector4_f32[2]);
		return vRet;
	}

	const Vec3 operator / (FXMVECTOR _other)
	{
		Vec3 vRet(_other.vector4_f32[0] == 0.f ? 0.f : x / _other.vector4_f32[0]
			, _other.vector4_f32[1] == 0.f ? 0.f : y / _other.vector4_f32[1]
			, _other.vector4_f32[2] == 0.f ? 0.f : z / _other.vector4_f32[2]);

		return vRet;
	}

	const Vec3 operator + (const Vec3& _other)
	{
		Vec3 vRet(x + _other.x, y + _other.y, z + _other.z);
		return vRet;
	}

	Vec3 operator + (float _f)
	{
		Vec3 vRet(x + _f, y + _f, z + _f);
		return vRet;
	}

	const Vec3 operator - (const Vec3& _other)
	{
		Vec3 vRet(x - _other.x, y - _other.y, z - _other.z);
		return vRet;
	}

	Vec3 operator - (float _f)
	{
		Vec3 vRet(x - _f, y - _f, z - _f);
		return vRet;
	}

	const Vec3 operator * (const Vec3& _other)
	{
		Vec3 vRet(x * _other.x, y * _other.y, z * _other.z);
		return vRet;
	}

	Vec3 operator * (float _f)
	{
		Vec3 vRet(x * _f, y * _f, z * _f);
		return vRet;
	}

	Vec3 operator * (float _f) const
	{
		Vec3 vRet(x * _f, y * _f, z * _f);
		return vRet;
	}

	Vec3 operator * (int _f)
	{
		Vec3 vRet(x * _f, y * _f, z * _f);
		return vRet;
	}

	const Vec3 operator / (const Vec3& _other)
	{
		Vec3 vRet(_other.x == 0.f ? 0.f : x / _other.x
			, _other.y == 0.f ? 0.f : y / _other.y
			, _other.z == 0.f ? 0.f : z / _other.z);

		return vRet;
	}

	Vec3 operator / (float _f)
	{
		Vec3 vRet(_f == 0.f ? 0.f : x / _f
			, _f == 0.f ? 0.f : y / _f
			, _f == 0.f ? 0.f : z / _f);

		return vRet;
	}

	const Vec3& operator += (const Vec3& _other)
	{
		x += _other.x;
		y += _other.y;
		z += _other.z;
		return (*this);
	}	

	const Vec3& operator += (float _f)
	{
		x += _f;
		y += _f;
		z += _f;
		return (*this);
	}


	const Vec3& operator *= (const Vec3& _other)
	{
		x *= _other.x;
		y *= _other.y;
		z *= _other.z;
		return (*this);
	}

	const Vec3& operator *= (float _f)
	{
		x *= _f;
		y *= _f;
		z *= _f;
		return (*this);
	}

	const Vec3& operator -= (const Vec3& _other)
	{
		x -= _other.x;
		y -= _other.y;
		z -= _other.z;
		return (*this);
	}

	const Vec3& operator -= (float _f)
	{
		x -= _f;
		y -= _f;
		z -= _f;
		return (*this);
	}

	const Vec3& operator /= (const Vec3& _other)
	{
		_other.x == 0.f ? x = 0.f : x /= _other.x;
		_other.y == 0.f ? y = 0.f : y /= _other.y;
		_other.z == 0.f ? z = 0.f : z /= _other.z;

		return (*this);
	}

	const Vec3& operator /= (float _f)
	{
		_f == 0.f ? x = 0.f : x /= _f;
		_f == 0.f ? y = 0.f : y /= _f;
		_f == 0.f ? z = 0.f : z /= _f;

		return (*this);
	}
};


typedef struct _tagVTX
{
	Vec3 vPos;
	Vec2 vUV;
	Vec4 vColor;
	Vec3 vNormal;
	Vec3 vTangent;
	Vec3 vBinormal;
	Vec4 vWeights;
	Vec4 vIndices;
}VTX;

typedef struct _tagIndex16
{
	WORD	_1, _2, _3;
	_tagIndex16()
		: _1(0), _2(0), _3(0)
	{}
	_tagIndex16(WORD __1, WORD __2, WORD __3)
		: _1(__1), _2(__2), _3(__3)
	{}

	static UINT size() { return sizeof(WORD); }
	static DXGI_FORMAT format() { return DXGI_FORMAT_R16_UINT; }
}INDEX16;

typedef struct _tagIndex32
{
	DWORD	_1, _2, _3;
	_tagIndex32()
		: _1(0), _2(0), _3(0)
	{}
	_tagIndex32(DWORD __1, DWORD __2, DWORD __3)
		: _1(__1), _2(__2), _3(__3)
	{}
	static UINT size() { return sizeof(DWORD); }
	static DXGI_FORMAT format() { return DXGI_FORMAT_R32_UINT; }
}INDEX32;

struct CBUFFER
{
	ID3D11Buffer*	pBuffer;
	UINT			iRegister;
	UINT			iSize;
};

struct tShaderParam
{
	SHADER_PARAM eShaderParam;
	UINT		 iRegister;
	UINT		 iTiming;
	wstring		 strParamName;
};

struct tLightCol
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
};

struct tLightInfo
{
	tLightCol	tCol;
	Vec4		vLightPos;
	Vec3		vLightDir;	 // 방향과 빛의 반경
	UINT		iLightType;
	Vec2		vSpotAngle;	 // x: 외부각, y: 내부각
	Vec2		vPad;
	Matrix		matVP;

	tLightInfo()
		: tCol{ Vec4(1.f, 1.f, 1.f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f) , Vec4(1.f, 1.f, 1.f, 1.f) }
		, iLightType((UINT)LIGHT_TYPE::DIR)
		, matVP()
	{}
	tLightInfo(const tLightCol& _tCol
		, const Vec4& _vLightPos, const Vec3& _vLightDir
		, UINT _iLightType)
		: tCol(_tCol)
		, vLightPos(_vLightPos)
		, vLightDir(_vLightDir)
		, iLightType(_iLightType)
		, matVP()
	{}
};

struct tLightArr
{
	tLightInfo  arrInfo[5];
	UINT        iLightCount;
	Vec3        vPad;
};

struct tRay
{
	Vec3 vOrigin;
	Vec3 vDir;
};

//==========
// 상수 버퍼
//==========
typedef struct _tagTransform
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matInvView;
	Matrix	matInvProj;
	Matrix  matPrevWVP;
	Vec4 g_fDelta;
}tTransform;

struct tAnimInfo
{
	Vec4 vUV;
	Vec4 vAnim;
};

//============
// FBX 구조체
//============

typedef struct _tagMtrlInfo
{
	Vec4 vMtrlDiff;
	Vec4 vMtrlSpec;
	Vec4 vMtrlAmb;
	Vec4 vMtrlEmiv;
	
	_tagMtrlInfo()
		: vMtrlDiff(1.f, 1.f, 1.f, 1.f)
		, vMtrlSpec(1.f, 1.f, 1.f, 1.f)
		, vMtrlAmb(1.f, 1.f, 1.f, 1.f)
		, vMtrlEmiv(1.f, 1.f, 1.f, 1.f)
	{}
}tMtrlInfo;

typedef struct _tagFbxMat
{
	tMtrlInfo tMtrl;
	wstring strMtrlName;
	wstring strDiff;
	wstring strNormal;
	wstring strSpec;
	wstring strEmis;
}tFbxMaterial;

typedef struct _tagWeightsAndIndices
{
	int		iBoneIdx;
	double	dWeight;
}tWeightsAndIndices;

typedef struct _tagContainer
{
	wstring								strName;
	vector<Vec3>						vecPos;
	vector<Vec3>						vecTangent;
	vector<Vec3>						vecBinormal;
	vector<Vec3>						vecNormal;
	vector<Vec2>						vecUV;

	vector<Vec4>						vecIndices;
	vector<Vec4>						vecWeights;

	vector<vector<UINT>>				vecIdx;
	vector<tFbxMaterial>				vecMtrl;

	// Animation 관련 정보
	bool								bAnimation;
	vector<vector<tWeightsAndIndices>>	vecWI;

	void Resize(UINT _iSize)
	{
		vecPos.resize(_iSize);
		vecTangent.resize(_iSize);
		vecBinormal.resize(_iSize);
		vecNormal.resize(_iSize);
		vecUV.resize(_iSize);
		vecIndices.resize(_iSize);
		vecWeights.resize(_iSize);
		vecWI.resize(_iSize);
	}

}tContainer;

// Animation
struct tMTKeyFrame
{
	float	fTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};

struct tMTBone
{
	wstring strBoneName;
	int iDepth;
	int iParentIndx;
	Matrix matOffset;
	vector<tMTKeyFrame> vecKeyFrame;
};

struct tMTAnimClip
{
	wstring strAnimName;
	int iStartFrame;
	int iEndFrame;
	int iFrameLength;
	
	double dStartTime;
	double dEndTime;
	double dTimeLength;

	FbxTime::EMode eMode;
};

struct tPathNode
{
	int iIdx;
	int iParentIdx;
	float fFScore; // 밑에 두개 더한거
	float fHScore; // 목표까지 예상 거리
	float fGScore; // 현재 노드까지 거리
	Vec3 vCenter;
};