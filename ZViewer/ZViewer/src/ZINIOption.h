#pragma once

#include <map>

class ZINIOption
{
public:
	ZINIOption();
	~ZINIOption();

	/// �����͸� �о�鿩�� ���� �����Ѵ�.
	void LoadFromFile(const std::string & strFilename);
	void SaveToFile(const std::string & strFilename);

	const std::string GetValue(const std::string & strKey);
private:
	std::map < std::string, std::string > m_iniMap;
};