#pragma once

typedef struct _tagBuildTask
{
	vector<string>	vecStrCpp;
	string			strFolderPath;
	string			strVcxprojPath;

	_tagBuildTask() {}
	_tagBuildTask(string& _strFolderPath, string& _strVcxprojPath)
		: strFolderPath(_strFolderPath), strVcxprojPath(_strVcxprojPath)
	{}

}tTask;

class CBuildMgr
{
private:
	vector<tTask*>		m_vecTask;
	int m_iUnityCppSize;

public:
	void progress();

public:
	void AddTask(string _strVcxprojPath, string _strFolderPath);
	void GetFolderName(const string& _strFolderPath, string& _strFolderNameOut);

private:
	XMLNode* FindCppNode(XMLNode* pNode);
	void ProcessUnityCpp(int _iIdx, XMLNode* _pRoot, tinyxml2::XMLDocument* _doc);
	void ExludeBuild(int _iIdx, XMLNode* _pRoot, tinyxml2::XMLDocument* _doc);
	bool CheckUnityData(UINT _iIdx);

public:
	static CBuildMgr* GetInst()
	{
		static CBuildMgr mgr;
		return &mgr;
	}

public:
	CBuildMgr();
	~CBuildMgr();
};

