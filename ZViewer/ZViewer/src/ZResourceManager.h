/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZResourceManager.h
*
*                                       http://www.wimy.com
*********************************************************************/


#pragma once

/// ���� ��� ������ ���� HINSTANCE �� ���ҽ��� �����ϴ� Ŭ����
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

	//const tstring GetString(UINT iStringID);

	HINSTANCE m_hInstance;

private:
	std::map < UINT, tstring > m_stringMap;	/// ID �� �ش��ϴ� String �� ĳ���Ѵ�.
};

