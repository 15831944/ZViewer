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

typedef std::map < std::string, std::string > iniMap;

class COptionFile
{

public:
	COptionFile();
	~COptionFile();

	/// �����͸� �о�鿩�� ���� �����Ѵ�.
	static void LoadFromFile(const std::string & strFilename, iniMap & settings);
	static void SaveToFile(const std::string & strFilename, const iniMap & settings);

private:
};