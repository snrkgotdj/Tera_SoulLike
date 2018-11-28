#pragma once

#include "global.h"

float getRandNum_float(float min, float max);
int   getRandNum_int(int _min, int _max);
Vec3  getRandNum_Vec3();
int   GetSizeofFormat(DXGI_FORMAT _eFormat);

template<typename T>
void Safe_Delete_Vec(vector<T>& _vec)
{
	for (UINT i = 0; i < _vec.size(); ++i)
	{
		SAFE_DELETE(_vec[i]);
	}
}

template<typename T>
void Safe_Delete_Vec_Res(vector<T>& _vec)
{
	for (UINT i = 0; i < _vec.size(); ++i)
	{
		_vec[i].Delete();
	}
}

template<typename T>
void Safe_Delete_List(list<T>& _list)
{
	list<T>::iterator iter = _list.begin();

	for (; iter != _list.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
}


template<typename T1, typename T2>
void Safe_Delete_Map(map<T1, T2>& _map)
{
	map<T1, T2>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}

template<typename T1, typename T2>
void Safe_Delete_Map_Res(map<T1, T2>& _map)
{
	map<T1, T2>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		iter->second.Delete();
	}
}

void SaveWString(const wstring& _str, FILE* _pFile);
wchar_t* LoadWString(FILE* _pFile);

class CRes;
template<typename T>
void SaveResourceKey(T* _pRes, FILE* _pFile)
{
	BYTE bRes = 0;
	if (NULL != _pRes)
		bRes = 1;

	fwrite(&bRes, 1, 1, _pFile);

	if (bRes)
	{
		SaveWString(_pRes->GetKey(), _pFile);	
	}
}

wchar_t* LoadResourceKey(FILE* _pFile);
	
Matrix GetRotMatirx(Vec3 _vLookAt);

Matrix GetMatrix(FbxAMatrix& _mat);

float Distance(Vec3& _vLeft, Vec3& _vRight);