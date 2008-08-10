/* ------------------------------------------------------------------------
 *
 * 2008. 8. 10 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include <vector>

/// ���� Ȯ���ڿ� ���� ������ �����ϴ� Ŭ����
class ExtInfoManager
{
public:
	static ExtInfoManager & GetInstance();

	std::vector < ExtSetting > m_extConnect;

	bool IsValidImageFileExt(const TCHAR * szFilename);

	const TCHAR * GetFileDlgFilter();

protected:
	ExtInfoManager();

	/// Ȯ���ڸ� ����
	void ExtMapInit();

	void _AddExtSet(const int iIconIndex, const TCHAR * ext);

	tstring m_strFileDlgFilter;

};