// UnityBuild.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tinyxml2.h"

#include "BuildMgr.h"

// cpp 파일명
vector<string> vecCppFiles;

void SearchVcxprojFile(string _strCurPath, string _strExt = "")
{
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA(string(_strCurPath + "*." + _strExt).c_str(), &fd);

	if (INVALID_HANDLE_VALUE == hFind)
		return;

	while (true)
	{
		// 폴더인 경우
		if (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes)
		{
			if (fd.cFileName[0] != '.')
				SearchVcxprojFile(_strCurPath + fd.cFileName + '\\', "vcxproj");			
		}		
		else
		{
			if (!strcmp("UnityBuild.vcxproj", fd.cFileName))
				break;

			// 파일중에 vcxproj 확장명인 파일이 있다면
			// 해당 폴더의 cpp 파일을 검출하여 UnityBuild 작업 목록을 하나 추가한다.
			CBuildMgr::GetInst()->AddTask(_strCurPath + fd.cFileName, _strCurPath);
			break;
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);
}


int main()
{
	/*FILE* pFile = NULL;
	_wfopen_s(&pFile, L"UnityBuild_01.cpp", L"wt+,ccs=UTF-16LE");
	fclose(pFile);*/

	char szDir[255] = "";
	GetCurrentDirectoryA(255, szDir);

	int iLen = strlen(szDir);

	for (int i = iLen - 1; i >= 0; --i)
	{
		if (szDir[i] == L'\\')
		{
			memset(szDir + i + 1, 0, (iLen - i - 1));
			break;
		}		
	}

	// 현재 디렉터리 경로
	string strFolrderPath = szDir;

	// unitybuild 를 사용할 프로젝트 폴더경로 입력.
	// 해당 프로젝트만 unitybuild 실행

	// all 키워드 입력시 모든 폴더 순회하면서 전체 프로젝트 전부 unitybuild 실행
	// 모든 폴더 순회, .vcproj 파일 찾기
	// .vcproj 파일을 찾은 폴더의 모든 cpp 파일 찾아서 vector 에 넣는다.
	SearchVcxprojFile(strFolrderPath);

	CBuildMgr::GetInst()->progress();

    return 0;
}

