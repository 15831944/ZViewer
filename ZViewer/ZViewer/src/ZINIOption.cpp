
#include "stdafx.h"
#include "ZINIOption.h"
#include <fstream>

using namespace std;

ZINIOption::ZINIOption()
{
}

ZINIOption::~ZINIOption()
{
}

void ZINIOption::LoadFromFile(const std::string & strFilename)
{
	ifstream fin(strFilename.c_str());

	char szData[256];
	while ( !fin.eof())
	{
		memset(szData, 0, sizeof(szData));

		fin.getline(szData, 256);

		if ( szData[0] != '\0' )	/// �����͸� ������ �ִ�.
		{
			if ( szData[0] == '#' || szData[0] == '/' )	// ó�� ����Ʈ�� # �̰ų� / �̸� �����Ѵ�.
			{
				continue;
			}

			if ( strlen(szData) <= 3 )	// �����ͷ� �� �� ���� ���̸� �����Ѵ�.
			{
				continue;
			}

			std::string strData = szData;

			std::string::size_type pos = strData.find("=");

			if ( pos == strData.npos )	// ó�� '=' �� ã�� �� ������ �̻��� ���̴�.
			{
				continue;
			}

			std::string strValue = strData.substr(pos+1);

			strData.resize(pos);
			std::string strKey = strData;

			_ASSERTE(strKey.empty() == false);
			_ASSERTE(strValue.empty() == false);

			m_iniMap[strKey] = strValue;
		}
	}



}

const std::string ZINIOption::GetValue(const std::string & strKey)
{
#ifdef _DEBUG
	if ( m_iniMap.count(strKey) <= 0 )
	{
		_ASSERTE(!"Can't find the key");
	}
	else
	{
		m_iniMap[strKey] = "Can't Find the string. Check language ini file.";
	}
#endif

	return m_iniMap[strKey];
}