/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MessageManager.cpp
 *
 * 2006.9.23 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "MessageManager.h"
#include "UnicodeFile.h"
#include "CommonFunc.h"

CMessageManager::CMessageManager()
{
}


CMessageManager::~CMessageManager()
{
}


/// � �� ������ �����Ѵ�.
void CMessageManager::SetLanguage(const std::string & lang)
{
	tstring strFileName = GetProgramFolder();
	strFileName += TEXT("\\language\\");


	if ( lang == "korean" )
	{
		strFileName = TEXT("korean.txt");
	}
	else /// �⺻���� �����̴�.
	{
		strFileName = TEXT("english.txt");
	}

}