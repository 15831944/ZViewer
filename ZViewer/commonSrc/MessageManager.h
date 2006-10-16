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

#include "OptionFile.h"

enum eLanguage
{
	eLanguage_ENGLISH,
	eLanguage_KOREAN,
};

/// ������ �޽����� ������ �ִٰ� �ʿ��� �� ��ȯ�ϴ� Ŭ����
class CMessageManager
{
public:
	~CMessageManager();

	static CMessageManager & getInstance();

	/// � �� ������ �����Ѵ�.
	void SetLanguage(eLanguage lang);

	/// �� �޽����� ��´�.
	const tstring & GetMessage(const tstring & key) const;

protected:

	CMessageManager();

	/// �ش� �� �´� ������ �о�鿩 �޽��� ���� �����Ѵ�.
	void _LoadLanguage(eLanguage lang);

	iniMap m_messageMap;

	mutable tstring m_errorMSg;
};

inline const TCHAR * GetMessage(const TCHAR * key)
{
	return CMessageManager::getInstance().GetMessage(key).c_str();
}