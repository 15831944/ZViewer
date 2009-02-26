/********************************************************************
*
* Created by zelon(Kim, Jinwook - Korea)
*
*   2005. 5. 7
*	ZMain.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once

#include "src/ZHistory.h"
#include "../commonSrc/DesktopWallPaper.h"
#include "../commonSrc/ZImage.h"
#include "src/ZCacheImage.h"

/// ������ ������ ���� ���� ����
enum eFileSortOrder
{
	eFileSortOrder_FILENAME,
	eFileSortOrder_LAST_MODIFY_TIME,
	eFileSortOrder_FILESIZE,
};

/// ��κ��� �۾��� ó���ϴ� ���� Ŭ����
class ZMain
{
	ZMain(void);
public:
	static ZMain & GetInstance();
	~ZMain(void);

	void StartSlideMode();

	/// Timer �� �޾��� ��
	void onTimer();

	/// ZViewer ���� �޽��� �ڽ�
	int ShowMessageBox(const TCHAR * msg, UINT button = MB_OK);

	/// ���� ȭ���� �׸� �� �ִ� ������ ũ�⸦ �޾ƿ´�.
	bool getCurrentScreenRect(RECT & rect);

	/// ���� ũ�⿡ �´� ShowWindow ũ�⸦ ���Ѵ�.
	void AdjustShowWindowScreen();

	/// On Window is resized
	void OnWindowResized();

	bool IsHandCursor() const { return m_bHandCursor; }
	void SetHandCursor(bool bHandCursor) { m_bHandCursor = bHandCursor; }

	void SetInitArg(const tstring & strArg) { m_strInitArg = strArg; }
	void RescanFolder();			///< ���ϸ���� �ٽ� �о���δ�.

	void OnInit();

	/// ���� ���� �ִ� ������ Exif ������ �����ش�.
	void ShowExif();

	void Draw(HDC toDrawDC = NULL, bool bEraseBg = true);

	/// ���纸�� �ִ� �̹����� ������ ����ȭ���� ������� �����Ѵ�.
	void SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style);

	/// ���� ��ġ�� ���� �̿� ���� - ���� ������ ���� ������ - �� ���´�.
	bool GetNeighborFolders(std::vector < tstring > & vFolders);

	/// ���� ������ �̵�
	void NextFolder();

	/// ���� ������ �̵�
	void PrevFolder();

	/// Ư�� ��ġ�� �̹��� ���Ϸ� �ǳʶڴ�.
	bool MoveIndex(int iIndex);

	int GetCalculatedMovedIndex(int iIndex);	///< ���� �̵��� �ε��� ��ȣ�� �ָ� ���� ��Ȳ�� �´� �ε��� ��ȣ�� �����ش�.

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

	void SetStatusHandle(HWND hWnd) { m_hStatusBar = hWnd; }
	HWND GetStatusHandle() const { return m_hStatusBar; }

	void StartTimer();
	void StopTimer();

	/// Cache status �� ���� ǥ���ٿ� ǥ���Ѵ�.
	void ShowCacheStatus();

	void ToggleAutoRotation();
	void ToggleAlwaysOnTop();

	/// ��üȭ�� ��带 ����Ѵ�.
	void ToggleFullScreen();

	void ToggleBigToScreenStretch();
	void ToggleSmallToScreenStretch();

	/// Loop Image �� ����Ѵ�.
	void ToggleLoopImage();

	int GetLogCacheHitRate() const;

	/// ���� ������ ���������� �����Ѵ�.
	void DeleteThisFileToRecycleBin();

	/// ���� ������ �����Ѵ�.
	void DeleteThisFile();

	/// ���� ������ �̵��Ѵ�.
	void MoveThisFile();

	/// ���� ������ �����Ѵ�.
	void CopyThisFile();

	void SetInstance(HINSTANCE hInstance) { m_hMainInstance = hInstance; }
	HINSTANCE GetHInstance() const { return m_hMainInstance; }
	void OnChangeCurrentSize(int iWidth, int iHeight);
	void OnDrag(int x, int y);

	void OnFocusLose();
	void OnFocusGet();

	void OnRightTopFirstDraw();

	/// Ư�� ������ ����.
	void OpenFile(const tstring & strFilename);

	/// Ư�� ������ ù��° ������ ����.
	void OpenFolder(const tstring & strFolder);
	void OpenFileDialog();

	/// ���� ������ �ٸ� �������� �����ϴ� ���� ���̾�α׸� ����.
	void SaveFileDialog();

	void Undo();
	void Redo();

	void ShowFileExtDlg();

	void ChangeFileSort(eFileSortOrder sortOrder);
	void ReLoadFileList();

	void LoadCurrent();

	/// �׸��� ������ �����츦 �����.
	void CreateShowWindow();

	/// ���� ǥ�� �����츦 �����.
	void CreateStatusBar();

	/// �޴� �� üũǥ�� �Ǵ� ���� Ȯ���Ͽ� �������ش�.
	void SetCheckMenus();

	void ZoomIn();
	void ZoomOut();

	void ZoomNone();

	HWND GetShowWindow() const { return m_hShowWindow; }

	/// ���纸�� �̹����� Ŭ�����忡 �����Ѵ�.
	void CopyToClipboard();
private:

	/// ���� �̹����� �巡���� �� �־, �ո���� Ŀ���ΰ�
	bool m_bHandCursor;

	void SetStatusBarText();
	void SetTitle();

	eOSKind m_osKind;

	void InitOpenFileDialog();

	typedef std::vector< FileData > FileListVector;

	void _GetFileListAndSort(const tstring & strFolderPathAndWildCard, FileListVector & vFileList);

	/// ���� ������ �������� �� ���� ó��. ���� ����, �̵� �Ŀ� �Ҹ��� �Լ��̴�.
	void _ProcAfterRemoveThisFile();

	/// �޴�, ���� ǥ���ٵ��� �����ش�.
	void FormShow();

	/// �޴�, ���� ǥ���ٵ��� �����.
	void FormHide();

	tstring m_strInitArg;			///< ���α׷� ���� ����.

	tstring m_strCurrentFolder;		///< ���� ����
	tstring m_strCurrentFilename;	///< ���� �����ְ� �ִ� �����̸�

	/// TODO: �� ������ ���� Ŭ������ ������� ��. �ֳ��ϸ� �� Ŭ���� ������ ���� ZCacheImage �� ����������ϱ� ������.
	FileListVector m_vFile;			///< ���� ������ ���ϵ� ���

	eFileSortOrder m_sortOrder;

	int m_iCurretFileIndex;

	/// ���� ���� ȭ�� ����. 1 �̸� 100%, 2 �̸� 200%, 0.1 �̸� 10%�� ũ��� ����. �׸��� m_currentImage �� ���� �� �ɼǿ� ���� ������ �����Ѵ�.
	double m_fCurrentZoomRate;

	/// ���� �����ְ� �ִ� �̹���
	ZImage * m_pCurrentImage;

	///< ���� �̹����� �ε��ϴ� �� �ɸ� �ð�
	DWORD m_dwLoadingTime;

	/// ���� �����쿡 ���� �ڵ�
	HWND m_hMainDlg;

	/// â �Ʒ��� ���� ǥ���ٿ� ���� �ڵ�
	HWND m_hStatusBar;

	HINSTANCE m_hMainInstance;			///< ���� �ν��Ͻ� �ڵ�

	HMENU m_hMainMenu;					///< ���� �޴��� ���� �ڵ�
	HMENU m_hPopupMenu;					///< �˾� �޴��� ���� �ڵ�

	int m_iShowingX;					///< �׸� �� ��� ��� �����ϳ�.
	int m_iShowingY;					///< �׸� �� ��� ��� �����ϳ�.

	RECT m_lastPosition;				///< Full Screen �� toggle �� �� full screen ���� â ��ġ

	static void FindFile(const TCHAR *path, std::vector< FileData > & foundStorage, bool bFindRecursive);
	static void FindFolders(const TCHAR *path, std::vector<tstring> & foundStorage, bool bFindRecursive = false);

	/// For Open File Dialog
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];       // buffer for file name
	//bool m_bOpeningFileDialog;	// ��üȭ���� �� ���� ���̾�α׸� ���� �����Ÿ��� ���� ����

	// For Undo/Redo
	ZHistory m_history;

	/// For Cache DC
	HDC m_hBufferDC;

	UINT_PTR m_timerPtr;		///< for timer

	/// ���۷� ���̴� DC �� �������Ѵ�.
	void _releaseBufferDC();

	/// Brush for Background
	HBRUSH m_bgBrush;

	/// ����� �����.
	void _eraseBackground(HDC mainDC, LONG right, LONG bottom);

	/// ������ �׸��� �׸� ȭ��â
	HWND m_hShowWindow;

	/// ����� ���� �� ���� Region
	HRGN m_hEraseRgb;

	/// ���纸�� �ִ� ȭ���� ���ʹ� �׸��� �� % �����ΰ� ���
	float m_fCenterX;
	float m_fCenterY;

	/// Zoom in & out ���� ���� ��ġ�� �׸��� �巡����. ZoomOut, ZoomIn �߿� ȣ���
	void _PositionPreviousCenter();

	/// ���� �̹����� �ε��Ǿ���
	bool m_bCurrentImageLoaded;
};
