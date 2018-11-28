// UnityBuild.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tinyxml2.h"

#include "BuildMgr.h"

// cpp ���ϸ�
vector<string> vecCppFiles;

void SearchVcxprojFile(string _strCurPath, string _strExt = "")
{
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA(string(_strCurPath + "*." + _strExt).c_str(), &fd);

	if (INVALID_HANDLE_VALUE == hFind)
		return;

	while (true)
	{
		// ������ ���
		if (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes)
		{
			if (fd.cFileName[0] != '.')
				SearchVcxprojFile(_strCurPath + fd.cFileName + '\\', "vcxproj");			
		}		
		else
		{
			if (!strcmp("UnityBuild.vcxproj", fd.cFileName))
				break;

			// �����߿� vcxproj Ȯ����� ������ �ִٸ�
			// �ش� ������ cpp ������ �����Ͽ� UnityBuild �۾� ����� �ϳ� �߰��Ѵ�.
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

	// ���� ���͸� ���
	string strFolrderPath = szDir;

	// unitybuild �� ����� ������Ʈ ������� �Է�.
	// �ش� ������Ʈ�� unitybuild ����

	// all Ű���� �Է½� ��� ���� ��ȸ�ϸ鼭 ��ü ������Ʈ ���� unitybuild ����
	// ��� ���� ��ȸ, .vcproj ���� ã��
	// .vcproj ������ ã�� ������ ��� cpp ���� ã�Ƽ� vector �� �ִ´�.
	SearchVcxprojFile(strFolrderPath);

	CBuildMgr::GetInst()->progress();

    return 0;
}

