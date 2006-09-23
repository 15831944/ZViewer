/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MessageManager.h
 *
 * 2006.9.23 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include <string>
#include <map>

/// ������ �޽����� ������ �ִٰ� �ʿ��� �� ��ȯ�ϴ� Ŭ����
class CMessageManager
{
public:
	CMessageManager();
	~CMessageManager();

	/// � �� ������ �����Ѵ�.
	void SetLanguage(const std::string & lang);

protected:

	std::map < tstring, tstring > m_messageMap;
};