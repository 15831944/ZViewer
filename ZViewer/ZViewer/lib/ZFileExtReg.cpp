/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtReg.cpp
*
*                                       http://www.wimy.com
*********************************************************************/
#include "stdafx.h"

#include "ZFileExtReg.H"
#include "CommonFunc.h"

bool ZFileExtReg::SetRegistries()
{
	RegSetExtension();
	RegSetDocumentType();

	return true;
}

bool ZFileExtReg::RegSetExtension()
{
	if( m_strExtension.empty() )
	{
		_ASSERTE(!"Extension string is empty!");
		return false;
	}

	std::string strKey = ".";
	strKey += m_strExtension;

	SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), "", m_strDocumentClassName.c_str());

	if( !m_strShellOpenCommand.empty() )
	{
		strKey += "\\shell\\open\\command";
		SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), "", m_strShellOpenCommand.c_str());
	}

	// Ȯ���ڿ� ���� �⺻ ���α׷��� ���Ѵ�.
	strKey = "SoftWare\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.";
	strKey += m_strExtension;
	SetRegistryValue(HKEY_CURRENT_USER, strKey.c_str(), "ProgID", m_strDocumentClassName.c_str());

	return TRUE;
}

bool  ZFileExtReg::RegSetDocumentType()
{
	if( m_strDocumentClassName.empty())
	{
		_ASSERTE(!"DocumentClassName string is empty!");
		return false;
	}

	std::string strKey = m_strDocumentClassName;

	SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), "", m_strDocumentDescription.c_str());

	// �⺻ ������ ���
	if( !m_strDocumentDefaultIcon.empty() )
	{
		strKey  = m_strDocumentClassName;
		strKey += "\\DefaultIcon";
		SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), "", m_strDocumentDefaultIcon.c_str());
	}

	// ���� Ŭ������ �� ���� ��ų ���
	if( !m_strShellOpenCommand.empty() )
	{
		strKey  = m_strDocumentClassName;
		strKey += "\\shell\\open\\command";
		SetRegistryValue(HKEY_CLASSES_ROOT, strKey.c_str(), "", m_strShellOpenCommand.c_str());
	}

	return true;
}

void ZFileExtReg::Clear()
{
	m_strExtension.resize(0);
	m_strContentType.resize(0);
	m_strShellOpenCommand.resize(0);
	m_strShellNewCommand.resize(0);
	m_strShellNewFileName.resize(0);

	m_strDocumentClassName.resize(0);
	m_strDocumentDescription.resize(0);
	m_strDocumentCLSID.resize(0);
	m_strDocumentCurrentVersion.resize(0);
	m_strDocumentDefaultIcon.resize(0);
	m_strDocumentShellOpenCommand.resize(0);
}