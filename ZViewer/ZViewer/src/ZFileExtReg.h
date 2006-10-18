/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtReg.h
*
*                                       http://www.wimy.com
*********************************************************************/
/*
 �� �ڵ�� http://www.codeproject.com/shell/cgfiletype.asp �� �����߽��ϴ�.
*/


#pragma once

#include <windows.h>
#include <string>

/// Ȯ���� ������, ���� Ŭ���� ���࿡ ���� ������ ������Ʈ���� ����ϴ� Ŭ����
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

	tstring		m_strExtension;
	tstring		m_strContentType;
	tstring		m_strShellOpenCommand;
	tstring		m_strShellNewCommand;
	tstring		m_strShellNewFileName;

	tstring		m_strDocumentClassName;
	tstring		m_strDocumentDescription;
	tstring		m_strDocumentCLSID;
	tstring		m_strDocumentCurrentVersion;
	tstring		m_strDocumentDefaultIcon;
	tstring		m_strDocumentShellOpenCommand;

};
