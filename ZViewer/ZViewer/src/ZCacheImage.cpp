
#include "stdafx.h"
#include "ZCacheImage.h"

ZCacheImage & ZCacheImage::GetInstance()
{
	static ZCacheImage ins;
	return ins;
}

ZCacheImage::ZCacheImage()
:	m_bNextCacheIsRight(true)	// ���������� ĳ�� ����
,	m_bGoOn(true)
,	m_iCacheLeftIndex(-1)
,	m_iCacheRightIndex(-1)
,	m_bUseCache(true)
,	m_iLogCacheHit(0)
,	m_iLogCacheMiss(0)
,	m_lCacheSize(0)
,	m_iMaxCacheImageNum(10)
,	m_iMaximumCacheMemoryMB(50)
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();

	InitializeCriticalSection(&m_lock);
}

ZCacheImage::~ZCacheImage()
{
	m_bGoOn = false;
	SetEvent(m_hEvent);

	WaitForSingleObject(m_hThread, INFINITE);

	OutputDebugString("Cached Thread ended.\r\n");

	CloseHandle(m_hThread);
	CloseHandle(m_hEvent);

	DeleteCriticalSection(&m_lock);
}

void ZCacheImage::SetImageVector(std::vector < std::string > & v)
{
	m_imageVector = v;
	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();
	m_lCacheSize = 0;

	for ( size_t i = 0; i < m_imageVector.size(); ++i)
	{
		m_imageMap[(int)i] = m_imageVector[i];
		m_imageMapRev[m_imageVector[i]] = (int)i;
	}

	char szTemp[256];
	sprintf(szTemp, "imageMapSize : %d\r\n", m_imageMap.size());
	OutputDebugString(szTemp);
	sprintf(szTemp, "imageVec : %d\r\n", m_imageVector.size());
	OutputDebugString(szTemp);
}

void ZCacheImage::StartThread()
{
	DWORD dwThreadID;
	m_hThread = CreateThread(0, 0, ThreadFuncProxy, this, 0, &dwThreadID);

	if ( SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL) == FALSE )
	{
		_ASSERTE(!"Can't SetThreadPriority!");
	}
}

DWORD ZCacheImage::ThreadFuncProxy(LPVOID p)
{
	ZCacheImage * pThis = (ZCacheImage*)p;

	pThis->ThreadFunc();

	return 0;
}

void ZCacheImage::CheckCacheDataAndClear()
{
	// ���� �ε����� �������� ������ ��� ���ϵ��� ĳ�ô� ������.

	CacheMapIterator it, endit;

	ZCacheLock lock;
	endit = m_cacheData.end();
	for ( it = m_cacheData.begin(); it != endit; )
	{
		if ( m_bNewChange )
		{
			return;
		}

		const std::string & strFilename = it->first;

		if ( m_imageMapRev[strFilename] < (m_iCurrentIndex - m_iMaxCacheImageNum) ||
			m_imageMapRev[strFilename] > (m_iCurrentIndex + m_iMaxCacheImageNum)

			)	// ĳ�� ������ �����
		{
			m_lCacheSize -= it->second.GetImageSize();
			m_cacheData.erase(it++);

#ifdef _DEBUG
			OutputDebugString("Clear one cached data\r\n");
#endif
		}
		else
		{
			++it;
		}
	}
}

bool ZCacheImage::CacheIndex(int iIndex)
{
	// �ִ� ĳ�� ũ�⸦ �Ѿ����� �� �̻� ĳ������ �ʴ´�.
	if ( (m_lCacheSize / 1024 / 1024) > m_iMaximumCacheMemoryMB )
	{
		return false;
	}

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_imageVector.size() ) iIndex = (int)m_imageVector.size() - 1;
	if ( iIndex == m_iCurrentIndex ) return true;

	// �̹� ĳ�õǾ� �ִ��� ã�´�.
	bool bFound = false;
	std::string strFileName;

	strFileName = m_imageMap[iIndex];
	{
		ZCacheLock lock;
		if ( m_cacheData.count(strFileName) > 0)
		{
			// found!
			bFound = true;
		}
	}

	if ( bFound == false )	// ĳ�õǾ� ���� ������ �о���δ�.
	{
		ZImage temp;
		if ( temp.LoadFromFile(strFileName) )
		{
			//if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
			
			AddCacheData(strFileName, temp);

			if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
		}
	}
	return true;
}

void ZCacheImage::ThreadFunc()
{
	m_bNewChange = false;
	CacheMapIterator it, endit;
	int iPos = 0;
	int i = 0;

	while ( m_bGoOn )
	{
		iPos = 1;
		_ASSERTE((int)m_imageVector.size() == (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_imageVector.size());

#ifdef _DEBUG
		if ( m_cacheData.empty() )
		{
			_ASSERTE(m_lCacheSize == 0);
		}
#endif

		/// ĳ�� �����͸� ���� ������ ĳ�ø� ����ش�.
		CheckCacheDataAndClear();

		for ( i=0; i<m_iMaxCacheImageNum/2; ++i)
		{
			if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
			
			// left side
			if ( !CacheIndex(m_iCurrentIndex - iPos) ) break;

			// right side
			if ( !CacheIndex(m_iCurrentIndex + iPos) ) break;

			++iPos;
		}

#ifdef _DEBUG
		OutputDebugString("wait event\r\n");
#endif

		WaitForSingleObject(m_hEvent, INFINITE);
		m_bNewChange = false;

#ifdef _DEBUG
		OutputDebugString("Recv event\r\n");
#endif
	}
}

bool ZCacheImage::hasCachedData(const std::string & strFilename, int iIndex)
{
	if ( ! m_bUseCache ) return false;

	// index �� üũ�Ѵ�.
	m_iCurrentIndex = iIndex;
	m_bNewChange = true;

	SetEvent(m_hEvent);

	{
		ZCacheLock lock;
		
		if ( m_cacheData.count(strFilename) > 0 ) return true;
	}
	return false;
}

void ZCacheImage::getCachedData(const std::string & strFilename, ZImage & image)
{
	CacheMapIterator it;
	ZCacheLock lock;
	
	it = m_cacheData.find(strFilename);
	if ( it == m_cacheData.end() )
	{
		_ASSERTE(!"Can't get NOT CACHED");
	}
	else
	{
		OutputDebugString("Cache hit\r\n");
	}
	image = it->second;
}


void ZCacheImage::AddCacheData(const std::string & strFilename, ZImage & image)
{
	ZCacheLock lock;

	/// �̹� ĳ�õǾ� ������ ĳ������ �ʴ´�.
	if ( m_cacheData.count(strFilename) > 0) return;

	/// �뷮�� üũ�ؼ� �� �̹����� ĳ������ �� ������ �Ѿ���� ĳ������ �ʴ´�.
	if ( (m_lCacheSize + image.GetImageSize()) /1024/1024 > m_iMaximumCacheMemoryMB ) return;

	m_cacheData[strFilename] = image;
	m_lCacheSize += m_cacheData[strFilename].GetImageSize();
}

long ZCacheImage::GetCachedKByte()
{
	/*
	ZCacheLock lock;
	CacheMapIterator it, endit = m_cacheData.end();

	unsigned long total = 0;

	for ( it = m_cacheData.begin(); it != endit; ++it)
	{
		ZImage & image = it->second;

		total += image.GetImageSize();
	}
	*/

	return (m_lCacheSize/1024);
}