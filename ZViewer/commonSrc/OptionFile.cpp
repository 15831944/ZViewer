/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	OptionFile.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "OptionFile.h"
#include "CommonFunc.h"
#include "UnicodeFile.h"
#include <fstream>

using namespace std;

/// ������ ������ �о�鿩�� ���� �����Ѵ�.
bool COptionFile::LoadFromFile(const tstring & strFilename, iniMap & settings)
{
	CUnicodeFile file;
	if ( false == file.open(strFilename, CUnicodeFile::eFileOpenMode_READ) )
	{
		return false;
	}

	tstring aLine;

	while ( file.getLine(aLine) )
	{
		if ( aLine.empty() ) continue;

		if ( aLine[0] == L'#' || aLine[0] == L'/' )	// ó�� ����Ʈ�� # �̰ų� / �̸� �����Ѵ�.
		{
			continue;
		}

		if ( aLine.size() <= 3 )	// �����ͷ� �� �� ���� ���̸� �����Ѵ�.
		{
			continue;
		}

		tstring::size_type pos = aLine.find(TEXT("="));

		if ( pos == aLine.npos )	// ó�� '=' �� ã�� �� ������ �̻��� ���̴�.
		{
			continue;
		}

		tstring strValue = aLine.substr(pos+1);
		tstring strKey = aLine.substr(0, pos);

		_ASSERTE(strKey.empty() == false);
		_ASSERTE(strValue.empty() == false);

		settings[strKey] = strValue;
	}
	return true;
}

/// �� ������ ������ ���Ͽ� ����.
void COptionFile::SaveToFile(const tstring & strFilename, const iniMap & settings)
{
	CUnicodeFile file;
	file.open(strFilename, CUnicodeFile::eFileOpenMode_WRITE);
	
	iniMap::const_iterator it;

	for ( it = settings.begin(); it != settings.end(); ++it )
	{
		const tstring key = it->first;
		const tstring value = it->second;

		tstring line = key;
		line += TEXT("=");
		line += value;

		file.writeLine(line);
	}
}
