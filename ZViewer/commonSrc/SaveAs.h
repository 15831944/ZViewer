/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * s:\ZViewer\ZViewer\src\SaveAs.h
 *
 * 2006. 2. 4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

/// �ٸ� �̸����� ������ ���� �� ���Ǵ� Ŭ����
class CSaveAs
{
    CSaveAs();

public:
    static CSaveAs & getInstance();
    ~CSaveAs();

	/// �θ� �������� �ڵ��� �����ش�.
	void setParentHWND(const HWND hwnd)
	{
		m_hParent = hwnd;
	}

	void setDefaultSaveFilename(const tstring & strInitialiFolder, const tstring & strFilename)
	{
		m_strInitialiFolder = strInitialiFolder;
		m_strSaveFileName = strFilename;
	}

	/// �ٸ� �̸����� ����â�� ����. ��ȯ���� false �̸� �������� �ʴ´�.
	bool showDialog();

	const TCHAR * getSaveFileName() const
	{
		return m_szFilenamebuf;
	}

protected:

	OPENFILENAME m_ofn;

	HWND m_hParent;
	tstring m_strInitialiFolder;
	tstring m_strSaveFileName;

	TCHAR m_szFilenamebuf[FILENAME_MAX];
};
