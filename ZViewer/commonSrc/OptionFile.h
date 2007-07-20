/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	OptionFile.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include <map>

typedef std::map < tstring, tstring > iniMap;

class COptionFile
{

public:
	COptionFile();
	~COptionFile();

	/// ������ ������ �о�鿩�� ���� �����Ѵ�.
	static bool LoadFromFile(const tstring & strFilename, iniMap & settings);

	/// �� ������ ������ ���Ͽ� ����.
	static void SaveToFile(const tstring & strFilename, const iniMap & settings);

private:
};