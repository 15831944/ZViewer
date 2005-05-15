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

#include "../../lib/ZImage.h"

// ĳ�ø� ��� ������� ������ ������, �켱���� �������� ����...
enum eLastActionDirection
{
	eLastActionDirection_FORWARD,	// PageDown ������ ���� ������ ���Ҵ�
	eLastActionDirection_BACKWARD,	// PageUp ������ ���� ������ ���Ҵ�.
};

class ZCacheImage
{
private:
	ZCacheImage();

public:
	static ZCacheImage & GetInstance();

	~ZCacheImage();

	inline CRITICAL_SECTION * GetLockPtr() { return &m_lock; }

	inline void LogCacheHit() { ++m_iLogCacheHit; }
	inline void LogCacheMiss() { ++m_iLogCacheMiss; }

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
	bool hasCachedData(const std::string & strFilename, int iIndex);
	void getCachedData(const std::string & strFilename, ZImage & image);
	void AddCacheData(const std::string & strFilename, ZImage & image);

	void SetLastActionDirection(eLastActionDirection last)
	{
		m_lastActionDirection = last;
	}

private:

	eLastActionDirection m_lastActionDirection;

	/// ĳ�õ� ������ �뷮
	long m_lCacheSize;

	/// ĳ�ð� hit �� Ƚ��
	unsigned int m_iLogCacheHit;
	/// ĳ�ð� miss �� Ƚ��
	unsigned int m_iLogCacheMiss;

	bool m_bUseCache;

	volatile bool m_bNewChange;
	CRITICAL_SECTION m_lock;

	/// �ִ� ĳ���� �̹��� ����. ���⼭ ���ݹ�ŭ ����, ���������� ����.
	const int m_iMaxCacheImageNum;

	/// �ִ� ĳ�� �뷮
	const int m_iMaximumCacheMemoryMB;

	/// threadfunc �� ��� �����ų ���ΰ�. ���α׷��� ���� �� false �� �������
	bool m_bGoOn;

	std::map < std::string, ZImage > m_cacheData;
	typedef std::map < std::string, ZImage >::iterator CacheMapIterator;

	size_t m_numImageVectorSize;

	std::map < int , std::string > m_imageMap;
	std::map < std::string, int > m_imageMapRev;

	/// ���纸�� �ִ� index;
	volatile int m_iCurrentIndex;

	HANDLE m_hThread;
	HANDLE m_hEvent;		// ĳ�� �̺�Ʈ

	/// ������ ��ȣ�� ������ ĳ���� �� ������ ĳ���Ѵ�.
	bool _CacheIndex(int iIndex);

	/// ĳ�õǾ� �ִ� �����͵� �� ���� �ε����κ��� ���� �ָ��ִ� �ε����� ��´�.
	int _GetFarthestIndexFromCurrentIndex();

	/// ���� CriticalSection Lock �� �ɱ� ���� Ŭ����
	class ZCacheLock
	{
	public:
		ZCacheLock()
		{
			EnterCriticalSection(ZCacheImage::GetInstance().GetLockPtr());
		}

		~ZCacheLock()
		{
			LeaveCriticalSection(ZCacheImage::GetInstance().GetLockPtr());
		}
	};

};

