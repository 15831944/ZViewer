#pragma once

#include "../../lib/ZImage.h"

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

	long GetCachedKByte();
	int GetLogCacheHitRate()
	{
		if ( (m_iLogCacheHit + m_iLogCacheMiss ) == 0 )
		{
			return 0;
		}
		return (m_iLogCacheHit * 100 / (m_iLogCacheMiss+m_iLogCacheHit)); 
	}

	void StartThread();

	size_t GetNumOfCacheImage() const { return m_cacheData.size(); 	}

	static DWORD WINAPI ThreadFuncProxy(LPVOID p);
	void ThreadFunc();

	void SetImageVector(std::vector < std::string > & v);
	bool hasCachedData(const std::string & strFilename, int iIndex);
	void getCachedData(const std::string & strFilename, ZImage & image);
	void AddCacheData(const std::string & strFilename, ZImage & image);

private:


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

	/// ĳ�ø� �¿�� �ϴµ�, ���� ĳ���� ���� ������ ���ΰ�?
	bool m_bNextCacheIsRight;

	int m_iCacheLeftIndex;
	int m_iCacheRightIndex;

	std::map < std::string, ZImage > m_cacheData;
	typedef std::map < std::string, ZImage >::iterator CacheMapIterator;

	std::vector < std::string > m_imageVector;
	std::map < int , std::string > m_imageMap;
	std::map < std::string, int > m_imageMapRev;

	/// ���纸�� �ִ� index;
	volatile int m_iCurrentIndex;

	/// ĳ���ϱ� ������ �ε���. �� ������ ��� ���� ����� ĳ�ø� �ٽ� �Ѵ�.
	int m_iCacheStartIndex;

	HANDLE m_hThread;
	HANDLE m_hEvent;		// ĳ�� �̺�Ʈ

private:
	/// ĳ�õ� �������� ��ȿ������ üũ�ؼ� ĳ�ø� ����ش�.
	//void CheckCacheDataAndClear();

	/// ������ ��ȣ�� ������ ĳ���� �� ������ ĳ���Ѵ�.
	bool CacheIndex(int iIndex);

	/// ĳ�õǾ� �ִ� �����͵� �� ���� �ε����κ��� ���� �ָ��ִ� �ε����� ��´�.
	int _GetFarthestIndexFromCurrentIndex();

private:

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

