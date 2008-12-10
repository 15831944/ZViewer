/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZCacheImage.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include "../../commonSrc/ZImage.h"
#include "../../commonSrc/LockUtil.h"

/// ĳ�ø� ��� ������� ������ ������, �켱���� �������� ����...
/**
 ���� ��� ����ڰ� PageDown ���� ���� �̹������� ���� �ִٸ� ���� �̹������� �� �� ĳ���س���,
 PageUp ���� ���� �̹��� �������� ���� �ִٸ� ���� �̹������� �� �� ĳ���س��� ���ؼ��̴�.
*/
enum eLastActionDirection
{
	eLastActionDirection_FORWARD,	///< PageDown ������ ���� ������ ���Ҵ�
	eLastActionDirection_BACKWARD,	///< PageUp ������ ���� ������ ���Ҵ�.
};


/// �̹����� ĳ���Ͽ� �����ϴ� Ŭ����
class ZCacheImage
{
private:
	ZCacheImage();

public:
	static ZCacheImage & GetInstance();

	~ZCacheImage();

	void CleanUp()
	{
		m_cacheData.clear();
	}

	inline void LogCacheHit() { ++m_iLogCacheHit; }
	inline void LogCacheMiss() { ++m_iLogCacheMiss; }

	/// ���� ������ ĳ���Ǿ����� üũ�ؼ� �����ش�.
	bool IsNextFileCached() const;

	void WaitCacheLock()
	{
		CLockObjUtil lock(m_cacheLock);
	}

	void debugShowCacheInfo();		///< ���� ĳ�� ������ ����� �ֿܼ� �����ش�. ������� ����

	void clearCache();
	void setCacheEvent()
	{
		m_hCacheEvent.setEvent();
	}

	long GetCachedKByte() const;
	int GetLogCacheHitRate() const
	{
		if ( (m_iLogCacheHit + m_iLogCacheMiss ) == 0 )
		{
			return 0;
		}
		return (m_iLogCacheHit * 100 / (m_iLogCacheMiss+m_iLogCacheHit)); 
	}

	void StartThread();

	// ���� ĳ�õǾ� �ִ� ���ϵ��� output ������� �ѷ��ش�.
	void ShowCachedImageToOutputWindow();

	size_t GetNumOfCacheImage() const { return m_cacheData.size(); 	}

	static DWORD WINAPI ThreadFuncProxy(LPVOID p);
	void ThreadFunc();

	void SetImageVector(const std::vector < FileData > & v);
	bool hasCachedData(const tstring & strFilename, int iIndex);	///< �� ���Ͽ� �ش��ϴ� ������ ĳ���ؼ� ������ �ִ��� üũ�ϴ� �Լ�
	void getCachedData(const tstring & strFilename, ZImage & image);	///< �� ���Ͽ� �ش��ϴ� ZImage ������ �޾ƿ��� �Լ�
	void AddCacheData(const tstring & strFilename, ZImage & image);		///< �� ���Ͽ� �ش��ϴ� ZImage ������ ���� �߰��ض�.

	void SetLastActionDirection(eLastActionDirection last)
	{
		m_lastActionDirection = last;
	}

	bool isCachingNow() const { return m_bNowCaching; }

private:

	/// ���� ĳ�� ���ΰ�...
	bool m_bNowCaching;

	/// ĳ���� ȿ������ ���ؼ� ����ڰ� ���������� ��� �������� ������������ ����س��´�.
	eLastActionDirection m_lastActionDirection;

	/// ĳ�õ� ������ �뷮
	long m_lCacheSize;

	/// ĳ�ð� hit �� Ƚ��. ����
	unsigned int m_iLogCacheHit;

	/// ĳ�ð� miss �� Ƚ��. ����
	unsigned int m_iLogCacheMiss;

	volatile bool m_bNewChange;

	mutable CLockObj m_cacheLock;

	/// threadfunc �� ��� �����ų ���ΰ�. ���α׷��� ���� �� false �� ���ָ� ĳ�� �����带 �ִ��� ���� ���� �� ����
	volatile bool m_bCacheGoOn;

	std::map < tstring, ZImage > m_cacheData;		///< ������ ĳ���� �����͸� ������ �ִ� ��
	typedef std::map < tstring, ZImage >::iterator CacheMapIterator;

	size_t m_numImageVectorSize;

	std::map < int , tstring > m_imageIndex2FilenameMap;	///< �̹��� ������ �ε��� ��ȣ,�����̸� ��
	std::map < tstring, int > m_imageFilename2IndexMap;		///< �̹��� �����̸�,�ε��� ��ȣ ��

	/// ���纸�� �ִ� index;
	volatile int m_iCurrentIndex;

	/// ���纸�� �ִ� �����̸�
	tstring m_strCurrentFileName;

	HANDLE m_hThread;			///< ĳ�� �����带 ����Ű�� �ڵ�
	
	/// ĳ�� �̺�Ʈ
	CEventObj m_hCacheEvent;

	/// ������ ��ȣ�� ������ ĳ���� �� ������ ĳ���Ѵ�.
	bool _CacheIndex(int iIndex);

	/// ĳ�õǾ� �ִ� �����͵� �� ���� �ε����κ��� ���� �ָ��ִ� �ε����� ��´�.
	int _GetFarthestIndexFromCurrentIndex();

	/*
	/// ���� ĳ�� �����忡�� �а� �ִ� �����̸�
	tstring m_cacheThreadReadingFileName;
	CLockObj m_cacheThreadReadingFileNameLock;	///< m_cacheThreadReadingFileName �� ���� lock ��ü

	/// ���� ���� �����忡�� �а� �ִ� �����̸�
	tstring m_mainThreadReadingFileName;
	CLockObj m_mainThreadReadingFileNameLock;	///< m_mainThreadReadingFileName �� ���� lock ��ü
	*/

};

