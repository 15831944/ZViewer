
#include "stdafx.h"
#include "OptionFile.h"
#include <fstream>

using namespace std;

void COptionFile::LoadFromFile(const std::string & strFilename, iniMap & settings)
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

			settings[strKey] = strValue;
		}
	}
}

void COptionFile::SaveToFile(const std::string & strFilename, const iniMap & settings)
{
	
}
