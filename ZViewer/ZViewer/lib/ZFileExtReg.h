/*
 �� �ڵ�� http://www.codeproject.com/shell/cgfiletype.asp �� �����߽��ϴ�.
*/


#pragma once

#include <windows.h>
#include <string>

class ZFileExtReg
{
public:

	ZFileExtReg()
	{
		Clear();
	}
	void Clear();

	bool SetRegistries();

	bool RegSetExtension();			/// HKEY_CLASSES_ROOT\.<Extension> �� ����Ѵ�.
	bool RegSetDocumentType();		/// DocumentType �� ����Ѵ�.

	bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData);

	std::string		m_strExtension;
	std::string		m_strContentType;
	std::string		m_strShellOpenCommand;
	std::string		m_strShellNewCommand;
	std::string		m_strShellNewFileName;

	std::string		m_strDocumentClassName;
	std::string		m_strDocumentDescription;
	std::string		m_strDocumentCLSID;
	std::string		m_strDocumentCurrentVersion;
	std::string		m_strDocumentDefaultIcon;
	std::string		m_strDocumentShellOpenCommand;

};
