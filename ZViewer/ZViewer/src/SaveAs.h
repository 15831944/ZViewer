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

	void setDefaultSaveFilename(const std::string & strInitialiFolder, const std::string & strFilename)
	{
		m_strInitialiFolder = strInitialiFolder;
		m_strSaveFileName = strFilename;
	}

	/// �ٸ� �̸����� ����â�� ����. ��ȯ���� false �̸� �������� �ʴ´�.
	bool showDialog();

	const char * getSaveFileName() const
	{
		return m_szFilenamebuf;
	}

protected:

	OPENFILENAME m_ofn;

	HWND m_hParent;
	std::string m_strInitialiFolder;
	std::string m_strSaveFileName;

	char m_szFilenamebuf[FILENAME_MAX];
};
