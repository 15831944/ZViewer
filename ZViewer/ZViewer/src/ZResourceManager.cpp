/*
2004. 12. 20 Kim Jinwook

Resource Manage for Language pack

*/

#include "stdafx.h"
#include "ZResourceManager.h"

ZResourceManager::ZResourceManager()
: m_hInstance(NULL)
{
}

ZResourceManager::~ZResourceManager()
{
}

ZResourceManager & ZResourceManager::GetInstance()
{
	static ZResourceManager instance;
	return instance;
}

const std::string ZResourceManager::GetString(UINT iStringID)
{
	if ( m_stringMap.find(iStringID ) == m_stringMap.end())
	{
		// �ʿ��� ã�� �� ������ LoadString �� �� �ʿ� �ִ´�.
		char szString[256] = { 0 };
		if ( 0 == LoadString(m_hInstance, iStringID, szString, 256) )
		{
			_ASSERTE(!"Can't get string");
			strcpy(szString, "Can't GET STRING");
		}

		std::string strString = szString;
		m_stringMap[iStringID] = strString;

		return strString;
	}

	// �ʿ� ������ �ʿ� �ִ� ��Ʈ���� ��ȯ�Ѵ�.
	return m_stringMap[iStringID];
}