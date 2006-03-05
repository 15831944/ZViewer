/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZMain.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once

#include "src/ZHistory.h"
#include "../lib/DesktopWallPaper.h"
#include "../lib/ZImage.h"
#include "src/ZCacheImage.h"

/// ������ ������ ���� ���� ����
enum eFileSortOrder
{
	eFileSortOrder_FILENAME,
	eFileSortOrder_LAST_MODIFY_TIME,
	eFileSortOrder_FILESIZE,
};

enum eOSKind
{
	eOSKind_UNKNOWN,
	eOSKind_98,
	eOSKind_2000,
	eOSKind_XP,
};

class ZMain
{
	ZMain(void);
public:
	static ZMain & GetInstance();
	~ZMain(void);

	bool IsHandCursor() const { return m_bHandCursor; }
	void SetHandCursor(bool bHandCursor) { m_bHandCursor = bHandCursor; }

	void SetInitArg(const std::string & strArg) { m_strInitArg = strArg; }
	void SetProgramFolder();
	void RescanFolder();			///< ���ϸ���� �ٽ� �о���δ�.
	void OnInit();
	void Draw(bool bEraseBg = true);

	/// ���纸�� �ִ� �̹����� ������ ����ȭ���� ������� �����Ѵ�.
	void SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style);
	
	/// ����ȭ�� �̹����� ���ش�.
	void ClearDesktopWallPaper();

	/// ���� ��ġ�� ���� �̿� ���� - ���� ������ ���� ������ - �� ���´�.
	bool GetNeighborFolders(std::vector < std::string > & vFolders);

	/// ���� ������ �̵�
	void NextFolder();

	/// ���� ������ �̵�
	void PrevFolder();

	/// Ư�� ��ġ�� �̹��� ���Ϸ� �ǳʶڴ�.
	bool MoveIndex(int iIndex);

	/// ���� ��ġ���� ������ �̵��Ѵ�.
	bool MoveRelateIndex(int iRelateIndex)
	{
		return MoveIndex(m_iCurretFileIndex + iRelateIndex);
	}

	/// ���� �̹��� ���Ϸ� �̵�
	bool NextImage()
	{
		ZCacheImage::GetInstance().SetLastActionDirection(eLastActionDirection_FORWARD);
		return MoveRelateIndex(+1);
	}

	/// ���� �̹��� ���Ϸ� �̵�
	bool PrevImage()
	{
		ZCacheImage::GetInstance().SetLastActionDirection(eLastActionDirection_BACKWARD);
		return MoveRelateIndex(-1);
	}

	/// ù��° �̹��� ���Ϸ� �̵�
	bool FirstImage();

	/// ������ �̹��� ���Ϸ� �̵�
	bool LastImage();

	/// ���纸�� �ִ� �̹��� ������ 90�� ȸ����Ŵ
	void Rotate(bool bClockWise);


	HWND GetHWND() const { return m_hMainDlg; }
	void SetHWND(HWND hWnd);
	void SetMainMenu(HMENU hMenu)
	{
		m_hMainMenu = hMenu;
	}
	void SetPopupMenu(HMENU hMenu)
	{
		m_hPopupMenu = hMenu;
	}

	long GetCachedKByte() const;

	void SetStatusHandle(HWND hWnd) { m_hStatus = hWnd; }
	HWND GetStatusHandle() const { return m_hStatus; }

	void ToggleFullScreen();

	void ToggleBigToScreenStretch();
	void ToggleSmallToScreenStretch();

	int GetLogCacheHitRate() const;

	void DeleteThisFile();
	void MoveThisFile();

	const std::string & GetProgramFolder() const	// ���α׷� ���� ������ �ִ� ������ �����´�.
	{
		return m_strProgramFolder;
	}

	void SetInstance(HINSTANCE hInstance) { m_hMainInstance = hInstance; }
	HINSTANCE GetHInstance() const { return m_hMainInstance; }
	void OnChangeCurrentSize(int iWidth, int iHeight);
	void OnDrag(int x, int y);

	void OnFocusLose();
	void OnFocusGet();

	void OnRightTopFirstDraw();

	/// Ư�� ������ ����.
	void OpenFile(const std::string & strFilename);

	/// Ư�� ������ ù��° ������ ����.
	void OpenFolder(const std::string strFolder);
	void OpenFileDialog();

	/// ���� ������ �ٸ� �������� �����ϴ� ���� ���̾�α׸� ����.
	void SaveFileDialog();

	void Undo();
	void Redo();

	void ShowFileExtDlg();

	/// �������� �۾� ǥ������ ���̰� ���ش�.
	void ShellTrayShow();

	void ChangeFileSort(eFileSortOrder sortOrder);
	void ReLoadFileList();

	void LoadCurrent();

private:

	/// ���� �̹����� �巡���� �� �־, �ո���� Ŀ���ΰ�
	bool m_bHandCursor;

	void SetStatusBarText();
	void SetTitle();

	BOOL _SetOSVersion();
	eOSKind m_osKind;

	void InitOpenFileDialog();

	typedef std::vector< FileData > FileListVector;

	void _GetFileListAndSort(const std::string & strFolderPathAndWildCard, FileListVector & vFileList);

	/// �������� �۾� ǥ������ �����.
	void ShellTrayHide();

	// ���� ������ �������� �� ���� ó��. ���� ����, �̵� �Ŀ� �Ҹ��� �Լ��̴�.
	void _ProcAfterRemoveThisFile();

	void FormShow();
	void FormHide();

	std::string m_strInitArg;			///< ���α׷� ���� ����.
	std::string m_strProgramFolder;		///< ���α׷� ���� ������ �ִ� ����

	std::string m_strCurrentFolder;		///< ���� ����
	std::string m_strCurrentFilename;	///< ���� �����ְ� �ִ� �����̸�
	
	FileListVector m_vFile;
	eFileSortOrder m_sortOrder;

	int m_iCurretFileIndex;

	int m_iCurrentScreenWidth;
	int m_iCurrentScreenHeight;

	/// ���� �����ְ� �ִ� �̹���
	ZImage m_currentImage;
	DWORD m_dwLoadingTime;				///< ���� �̹����� �ε��ϴ� �� �ɸ� �ð�

	HWND m_hMainDlg;
	HWND m_hStatus;
	HINSTANCE m_hMainInstance;

	HMENU m_hMainMenu;
	HMENU m_hPopupMenu;

	int m_iShowingX;					///< �׸� �� ��� ��� �����ϳ�.
	int m_iShowingY;					///< �׸� �� ��� ��� �����ϳ�.

	void ZFindFile(const char *path, std::vector< FileData > & foundStorage, bool bFindRecursive);
	void ZFindFolders(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive = false);

	// For Open File Dialog
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[MAX_PATH];       // buffer for file name
	//bool m_bOpeningFileDialog;	// ��üȭ���� �� ���� ���̾�α׸� ���� �����Ÿ��� ���� ����

	// For Undo/Redo
	ZHistory m_history;

};
