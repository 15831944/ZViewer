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

	void debugShowCacheInfo();

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
	bool hasCachedData(const tstring & strFilename, int iIndex);
	void getCachedData(const tstring & strFilename, ZImage & image);
	void AddCacheData(const tstring & strFilename, ZImage & image);

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

	/// ĳ�ð� hit �� Ƚ��
	unsigned int m_iLogCacheHit;

	/// ĳ�ð� miss �� Ƚ��
	unsigned int m_iLogCacheMiss;

	volatile bool m_bNewChange;

	CLockObj m_cacheLock;

	/// threadfunc �� ��� �����ų ���ΰ�. ���α׷��� ���� �� false �� �������
	bool m_bGoOn;

	std::map < tstring, ZImage > m_cacheData;
	typedef std::map < tstring, ZImage >::iterator CacheMapIterator;

	size_t m_numImageVectorSize;

	std::map < int , tstring > m_imageMap;
	std::map < tstring, int > m_imageMapRev;

	/// ���纸�� �ִ� index;
	volatile int m_iCurrentIndex;

	HANDLE m_hThread;
	
	/// ĳ�� �̺�Ʈ
	CEventObj m_hCacheEvent;

	/// ������ ��ȣ�� ������ ĳ���� �� ������ ĳ���Ѵ�.
	bool _CacheIndex(int iIndex);

	/// ĳ�õǾ� �ִ� �����͵� �� ���� �ε����κ��� ���� �ָ��ִ� �ε����� ��´�.
	int _GetFarthestIndexFromCurrentIndex();
};

