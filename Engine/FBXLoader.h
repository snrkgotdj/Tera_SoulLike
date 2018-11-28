#pragma once

#pragma comment(lib, "libfbxsdk-md.lib")
#include "fbxsdk.h"
#include "global.h"

struct tKeyFrame
{
	FbxAMatrix  matTransform;
	double		dTime;
};

struct tBone
{
	wstring		strBoneName;
	int			iDepth;				// 계층구조 깊이
	int			iParentIdx;			// 부모 Bone 의 인덱스
	FbxAMatrix  matOffset;			// Offset 행렬( -> 뿌리 -> Local)	
	FbxAMatrix	matBone;
	vector<tKeyFrame> vecKeyFrame;	// 키 프레임 별 행렬 정보	
};

struct tAnimClip
{
	wstring		strClipName;
	FbxTime		tStartTime;
	FbxTime		tEndTime;
	FbxLongLong llLength;
	FbxTime::EMode eMode;
};

class CMesh;

class CFBXLoader
{
private:
	FbxManager*						m_pManager;
	FbxScene*						m_pScene;
	FbxImporter*					m_pImporter;

	vector<tContainer>				m_vecContainer;

	// Animation
	vector<tBone*>					m_vecBone;
	FbxArray<FbxString*>			m_arrAnimName;
	vector<tAnimClip*>				m_vecAnimClip;

public:
	void init();
	void LoadFbx(const wstring& _strPath);

public:
	int GetContainerCount() { return (int)m_vecContainer.size(); }
	const tContainer& GetContainer(int _iIdx) { return m_vecContainer[_iIdx]; }
	vector<tBone*>& GetBones() { return m_vecBone; }
	vector<tAnimClip*> GetAnimClip() { return m_vecAnimClip; }

private:
	void LoadMeshDataFromNode(FbxNode* _pRoot);
	void LoadMesh(FbxMesh* _pFbxMesh);
	void LoadMaterial(FbxSurfaceMaterial* _pMtrlSur);

	void GetTangent(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);
	void GetBinormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);
	void GetNormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);
	void GetUV(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder);

	Vec4 GetMtrlData(FbxSurfaceMaterial * _pSurface, const char* _pMtrlName, const char* _pMtrlFactorName);
	wstring GetMtrlTextureName(FbxSurfaceMaterial * _pSurface, const char* _pMtrlProperty);

	void LoadTexture();
	void CreateMaterial();

	// Animation
	void LoadSkeleton(FbxNode* _pNode);
	void LoadSkeleton_Re(FbxNode* _pNode, int _iDepth, int _iIdx, int _iParentIdx);
	void LoadAnimationClip();
	void Triangulate(FbxNode* _pNode);

	void LoadAnimationData(FbxMesh* _pMesh, tContainer* _pContainer);
	void LoadWeightsAndIndices(FbxCluster* _pCluster, int _iBoneIdx, tContainer* _pContainer);
	void LoadOffsetMatrix(FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform, int _iBoneIdx, tContainer* _pContainer);
	void LoadKeyframeTransform(FbxNode* _pNode, FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform
		, int _iBoneIdx, tContainer* _pContainer);

	int FindBoneIndex(string _strBoneName);
	FbxAMatrix GetTransform(FbxNode* _pNode);

	void CheckWeightAndIndices(FbxMesh* _pMesh, tContainer * _pContainer);

public:
	CFBXLoader();
	~CFBXLoader();
};

