/*
 2004. 12. 20 Kim Jinwook

  Resource Manage for Language pack

*/

#pragma once

class ZResourceManager
{
	ZResourceManager();

public:
	~ZResourceManager();

	static ZResourceManager & GetInstance();

	void SetHandleInstance(HINSTANCE hInstance)
	{
		m_hInstance = hInstance;
	}

	HINSTANCE GetHInstance() const
	{
		return m_hInstance;
	}

	const std::string GetString(UINT iStringID);

	HINSTANCE m_hInstance;

private:
	std::map < UINT, std::string > m_stringMap;	/// ID �� �ش��ϴ� String �� ĳ���Ѵ�.
};