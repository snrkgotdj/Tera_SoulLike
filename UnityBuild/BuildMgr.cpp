#include "stdafx.h"
#include "BuildMgr.h"

#include "tinyxml2.h"
#include <assert.h>

CBuildMgr::CBuildMgr()
	:m_iUnityCppSize(30)
{
}

CBuildMgr::~CBuildMgr()
{
	for (UINT i = 0; i < m_vecTask.size(); ++i)
	{
		delete m_vecTask[i];
	}
}

void CBuildMgr::AddTask(string _strVcxprojPath, string _strFolderPath)
{
	// �����۾� �ϳ��� �Ŵ����� �߰��Ѵ�.
	tTask* pNewTask = new tTask(_strFolderPath , _strVcxprojPath);

	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA(string(_strFolderPath + "*.cpp").c_str(), &fd);

	if (INVALID_HANDLE_VALUE == hFind)
		return;

	char szFileName[50] = "";

	while (true)
	{
		if (fd.cFileName[0] != '.')
		{
			// ���� ��
			int iLen = (int)strlen(fd.cFileName);
			memset(szFileName, 0, 50);
			//�����̸� ���̰� 12���� ũ�� unity�������� Ȯ��
			if (iLen >= 10)
			{
				for (int i = iLen - 1; i >= 0; --i)
				{
					if (fd.cFileName[i] == '_')
					{
						memcpy_s(szFileName, 50, fd.cFileName + i + 1, iLen - 1 - i);
						break;
					}
				}
			}

			// Unity������ �ƴҰ�� 
			string strFileName = szFileName;
			if (strFileName != "Unity.cpp")
			{
				// cpp ���� �̸����� ���Ϳ� ����Ѵ�.
				pNewTask->vecStrCpp.push_back(fd.cFileName);
			}
			
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	m_vecTask.push_back(pNewTask);
}

void CBuildMgr::GetFolderName(const string& _strFolderPath, string& _strFolderNameOut)
{
	char szForderPath[255] = "";
	char szForderName[50] = "";
	strcpy_s(szForderPath, 255, _strFolderPath.c_str());
	int iLen = (int)strlen(_strFolderPath.c_str());
	for (int j = iLen - 2; j >= 0; --j)
	{
		if (L'\\' == szForderPath[j])
		{
			memcpy_s(szForderName, 50, szForderPath + j + 1, iLen - 2 - j);
			break;
		}
	}

	_strFolderNameOut = szForderName;
}

XMLNode * CBuildMgr::FindCppNode(XMLNode * pNode)
{
	XMLNode* pChild = pNode->FirstChild();

	XMLDeclaration* pDecl = pNode->ToDeclaration();
	XMLElement* pElem = pNode->ToElement();
	XMLComment* pCom = pNode->ToComment();
	XMLAttribute* pAtt = NULL;
	if (pElem)
		(XMLAttribute*)pElem->FirstAttribute();

	XMLText* pText = NULL;
	
	pChild = pChild->NextSibling()->FirstChild();

	// ������Ʈ ���
	while (true)
	{
		pChild = pChild->NextSibling();

		if (NULL == pChild)
			break;

		if (!strcmp(pChild->Value(), "ItemGroup"))
		{
			if (NULL != pChild->FirstChild() && !strcmp(pChild->FirstChild()->Value(), "ClCompile"))
				return pChild;
		}		
	}
	
	return nullptr;
}

void CBuildMgr::ProcessUnityCpp(int _iIdx, XMLNode* _pRoot, tinyxml2::XMLDocument* _doc)
{
	FILE* pFile = nullptr;

	string strName;
	GetFolderName(m_vecTask[_iIdx]->strFolderPath, strName);

	int iCppSize = (int)m_vecTask[_iIdx]->vecStrCpp.size();
	int iUnityCppSize = iCppSize / m_iUnityCppSize + 1;

	// Unity Cpp Insert
	XMLNode* pParent = _pRoot;
	XMLNode* pCppParent = FindCppNode(pParent);
	XMLNode* pCppNode = FindCppNode(pParent)->FirstChild();
	XMLNode* pDeleteNode = pCppNode;

	// Unity cpp ���δ� include ����
	while (true)
	{
		const char* pValue = ((XMLElement*)pDeleteNode->ToElement())->FirstAttribute()->Value();

		int iLen = (int)strlen(pValue);
		char szFileName[50] = "";
		XMLNode* pNext = pDeleteNode->NextSibling();
		for (int j = iLen - 1; j >= 0; --j)
		{
			if (pValue[j] == '_')
			{
				// UnityCpp�ϰ�� �������� �ʰ� �������� �̵�
				memcpy_s(szFileName, 50, pValue + j + 1, iLen - 1 - j);
				string strFileName = szFileName;
				if (strFileName == "Unity.cpp")
				{
					pCppParent->DeleteChild(pDeleteNode);
					break;
				}
			}
		}

		pDeleteNode = pNext;
		if (pDeleteNode == nullptr)
			break;
	}

	// ������ Unity Cpp�� include �߰�
	for (int j = 0; j < iUnityCppSize; ++j)
	{
		char szCppName[50] = "";
		wsprintfA(szCppName, "_%d_Unity.cpp", j);
		string strCPP = m_vecTask[_iIdx]->strFolderPath + strName + szCppName;

		fopen_s(&pFile, strCPP.c_str(), "w");

		// cpp Include
		for (int k = 0; k < m_iUnityCppSize; ++k)
		{
			if (j * m_iUnityCppSize + k >= (int)m_vecTask[_iIdx]->vecStrCpp.size())
				break;

			fprintf_s(pFile, "#include \"");
			int a = j * m_iUnityCppSize + k;
			fprintf_s(pFile, m_vecTask[_iIdx]->vecStrCpp[a].c_str());
			fprintf_s(pFile, "\"\n");
		}
		fclose(pFile);

		string strComment;
		strComment = "ClCompile Include=\"" + strName + szCppName + '"';

		string pValue = ((XMLElement*)pCppNode->ToElement())->FirstAttribute()->Value();

		if (pValue != strName + szCppName)
		{
			XMLElement* pIncludCpp = _doc->NewElement("ClCompile");
			pIncludCpp->SetAttribute("Include", (strName + szCppName).c_str());
			pCppParent->InsertEndChild(pIncludCpp);

			//// �⺻�������� ����
			//// ��� ����
			XMLElement* pExcludeDebug = _doc->NewElement("ExcludedFromBuild");
			XMLElement* pExcludeRelease = _doc->NewElement("ExcludedFromBuild");

			// Text ����
			pExcludeDebug->SetText("true");
			pExcludeRelease->SetText("true");

			// �Ӽ��� ����
			pExcludeDebug->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Debug|Win32'");
			pExcludeRelease->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Release|Win32'");
	
			pIncludCpp->InsertFirstChild(pExcludeDebug);
			pIncludCpp->InsertEndChild(pExcludeRelease);

			string path = m_vecTask[_iIdx]->strVcxprojPath.c_str();
			_doc->SaveFile(path.c_str());
		}
	}
}

void CBuildMgr::ExludeBuild(int _iIdx, XMLNode* _pRoot, tinyxml2::XMLDocument* _doc)
{

	XMLNode* pParent = _pRoot;

	// cpp ���� ���� ����
	XMLNode* pCppNode = FindCppNode(pParent);
	pCppNode = pCppNode->FirstChild();

	//// test.cpp ���� ����
	while (true)
	{
		const char* pValue = ((XMLElement*)pCppNode->ToElement())->FirstAttribute()->Value();

		int iLen = (int)strlen(pValue);
		char szFileName[50] = "";
		for (int j = iLen - 1; j >= 0; --j)
		{
			// �������� ������ �ƴҰ�� �������� �̵�
			if (pValue[j] == L'\\')
				goto next;

			else if (pValue[j] == '_')
			{
				// UnityCpp�ϰ�� �������� �ʰ� �������� �̵�
				memcpy_s(szFileName, 50, pValue + j + 1, iLen - 1 - j);		
				string strFileName = szFileName;
				if (strFileName == "Unity.cpp")
				{
					goto next;
				}
			}	
		}
		if (!strcmp(pValue,"stdafx.cpp"))
		{
			// ����Ƽ ��������� üũ
			XMLNode* pCheckNode = pCppNode->LastChild()->PreviousSibling();
			const char* pDebugValue = pCheckNode->Value();
			if (!strcmp(pDebugValue, "ExcludedFromBuild"))
			{
				const char* pDebugElement = ((XMLElement*)pCheckNode->ToElement())->FirstAttribute()->Value();
				if (!strcmp(pDebugElement, "'$(Configuration)|$(Platform)'=='Unity_Debug|Win32'"))
				{
					((XMLElement*)pCheckNode->ToElement())->SetText("true");

					pCheckNode = pCheckNode->NextSibling();
					const char* pReleaseElement = ((XMLElement*)pCheckNode->ToElement())->FirstAttribute()->Value();
					if (!strcmp(pReleaseElement, "'$(Configuration)|$(Platform)'=='Unity_Release|Win32'"))
					{
						((XMLElement*)pCheckNode->ToElement())->SetText("true");
					}
					else 
					{
						XMLElement* pExcludeRelease = _doc->NewElement("ExcludedFromBuild");
						pExcludeRelease->SetText("true");
						pExcludeRelease->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Unity_Release|Win32'");
						pCppNode->InsertEndChild(pExcludeRelease);
					}
				}

				else
				{
					XMLElement* pExcludeDebug = _doc->NewElement("ExcludedFromBuild");
					XMLElement* pExcludeRelease = _doc->NewElement("ExcludedFromBuild");

					// Text ����
					pExcludeDebug->SetText("true");
					pExcludeRelease->SetText("true");

					// �Ӽ��� ����
					pExcludeDebug->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Unity_Debug|Win32'");
					pExcludeRelease->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Unity_Release|Win32'");

					pCppNode->InsertEndChild(pExcludeDebug);
					pCppNode->InsertEndChild(pExcludeRelease);
				}
			}	
			else
			{
				XMLElement* pExcludeDebug = _doc->NewElement("ExcludedFromBuild");
				XMLElement* pExcludeRelease = _doc->NewElement("ExcludedFromBuild");

				// Text ����
				pExcludeDebug->SetText("true");
				pExcludeRelease->SetText("true");

				// �Ӽ��� ����
				pExcludeDebug->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Unity_Debug|Win32'");
				pExcludeRelease->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Unity_Release|Win32'");

				pCppNode->InsertEndChild(pExcludeDebug);
				pCppNode->InsertEndChild(pExcludeRelease);
			}
		}
		else
		{
			// ��� ����
			XMLElement* pExcludeDebug = _doc->NewElement("ExcludedFromBuild");
			XMLElement* pExcludeRelease = _doc->NewElement("ExcludedFromBuild");

			// Text ����
			pExcludeDebug->SetText("true");
			pExcludeRelease->SetText("true");

			// �Ӽ��� ����
			pExcludeDebug->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Unity_Debug|Win32'");
			pExcludeRelease->SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='Unity_Release|Win32'");

			pCppNode->DeleteChildren();
			pCppNode->InsertFirstChild(pExcludeDebug);
			pCppNode->InsertEndChild(pExcludeRelease);
		}
		

	next:
		pCppNode = pCppNode->NextSibling();
		if (pCppNode == nullptr)
			break;
	}
}

bool CBuildMgr::CheckUnityData(UINT _iIdx)
{
	FILE* pFile = nullptr;
	string strUnityData = m_vecTask[_iIdx]->strFolderPath;
	strUnityData += "UnityData.bin";

	fopen_s(&pFile, strUnityData.c_str(), "rb+");
	if (nullptr == pFile)
	{
		fopen_s(&pFile, strUnityData.c_str(), "wb");
		fclose(pFile);
		pFile = nullptr;

		fopen_s(&pFile, strUnityData.c_str(), "rb+");
		if (nullptr == pFile)
			assert(NULL);
	}
	UINT iCppSize = 0;
	fread(&iCppSize, sizeof(UINT), 1, pFile);

	if (iCppSize != m_vecTask[_iIdx]->vecStrCpp.size())
	{
		fclose(pFile);
		pFile = nullptr;
		// �ٸ���� �����
		fopen_s(&pFile, strUnityData.c_str(), "wb");
		iCppSize = (UINT)m_vecTask[_iIdx]->vecStrCpp.size();
		fwrite(&iCppSize, sizeof(UINT), 1, pFile);
		fclose(pFile);
		return true;
	}
	
	fclose(pFile);
	return false;
}

void CBuildMgr::progress()
{
	// �۾� ����
	// cpp ���忡�� ����
	
	for (UINT i = 0; i < m_vecTask.size(); ++i)
	{
		tinyxml2::XMLDocument doc;
		doc.LoadFile(m_vecTask[i]->strVcxprojPath.c_str());

		XMLNode* pParent = &doc;

		// ���� Cpp���� Ȯ��, ������� ������ ��Ƽ��
		bool iResult = CheckUnityData(i);
		if (false == iResult)
			continue;

		//// ���忡�� ����
		ExludeBuild(i, pParent, &doc);

		//// ������ Unity Cpp���� �� �߰�
		ProcessUnityCpp(i, pParent, &doc);
		
		// ���� ����
		string path = m_vecTask[i]->strVcxprojPath.c_str();
		//path += ".xml";
		doc.SaveFile(path.c_str());
	}
}