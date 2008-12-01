/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
*
*   2005. 5. 7
*	ZCacheImage.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "ZCacheImage.h"
#include "ZOption.h"
#include "ZMain.h"

using namespace std;

ZCacheImage & ZCacheImage::GetInstance()
{
	static ZCacheImage ins;
	return ins;
}

ZCacheImage::ZCacheImage()
:	m_bGoOn(true)
,	m_iLogCacheHit(0)
,	m_iLogCacheMiss(0)
,	m_lCacheSize(0)
,	m_numImageVectorSize(0)
,	m_lastActionDirection(eLastActionDirection_FORWARD)
{
	m_bNowCaching = false;
	m_hThread = INVALID_HANDLE_VALUE;
	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();

}

ZCacheImage::~ZCacheImage()
{
	m_bGoOn = false;

	m_hCacheEvent.setEvent();

	if ( m_hThread != INVALID_HANDLE_VALUE )
	{
		/// ĳ�� �����尡 �����ϱ� ��ٸ���.
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);

		m_hThread = INVALID_HANDLE_VALUE;
	}

	DebugPrintf(TEXT("Cached Thread ended."));
}

void ZCacheImage::SetImageVector(const std::vector < FileData > & v)
{
	CLockObjUtil lock(m_cacheLock);

	m_numImageVectorSize = v.size();

	m_imageMap.clear();
	m_imageMapRev.clear();
	m_cacheData.clear();
	m_lCacheSize = 0;

	for ( size_t i = 0; i < m_numImageVectorSize; ++i)
	{
		m_imageMap.insert(std::make_pair((int)i, v[i].m_strFileName));
		m_imageMapRev.insert(std::make_pair(v[i].m_strFileName, (int)i));
	}

	DebugPrintf(TEXT("imageMapSize : %d"), m_imageMap.size());
	DebugPrintf(TEXT("imageVecSize : %d"), m_numImageVectorSize);
}

void ZCacheImage::StartThread()
{
	if ( ZOption::GetInstance().IsUseCache() )
	{
		DWORD dwThreadID;
		m_hThread = CreateThread(0, 0, ThreadFuncProxy, this, 0, &dwThreadID);

		// Cache �� �����ϴ� �������
		if ( SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL) == FALSE )
		{
			_ASSERTE(!"Can't SetThreadPriority!");
		}
	}
}

// ���� ĳ�õǾ� �ִ� ���ϵ��� output ������� �ѷ��ش�.
void ZCacheImage::ShowCachedImageToOutputWindow()
{
#ifndef _DEBUG
	return; // ������ ��忡���� �׳� ����
#endif

	CLockObjUtil lock(m_cacheLock);

	CacheMapIterator it, endIt = m_cacheData.end();

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		DebugPrintf(TEXT("Cacaed Filename : %s"), it->first.c_str());
	}
}

DWORD ZCacheImage::ThreadFuncProxy(LPVOID p)
{
	ZCacheImage * pThis = (ZCacheImage*)p;

	pThis->ThreadFunc();

	return 0;
}

/// ĳ�õǾ� �ִ� �����͵� �� ���� �ε����κ��� ���� �ָ��ִ� �ε����� ��´�.
int ZCacheImage::_GetFarthestIndexFromCurrentIndex()
{
	CLockObjUtil lock(m_cacheLock);

	CacheMapIterator it, endIt = m_cacheData.end();

	int iFarthestIndex = m_iCurrentIndex;
	int iDistanceMax = 0;
	int iDistance = 0;
	int iTempIndex = 0;

	tstring strFarthest;

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		iTempIndex = m_imageMapRev[it->first];
		iDistance = abs(iTempIndex - m_iCurrentIndex);

		if ( iDistance > iDistanceMax )
		{
			iDistanceMax = iDistance;
			iFarthestIndex = iTempIndex;
			strFarthest = it->first;
		}
	}

	_ASSERTE(iFarthestIndex >= 0 );

	return iFarthestIndex;
}

bool ZCacheImage::_CacheIndex(int iIndex)
{
	// �ִ� ĳ�� ũ�⸦ �Ѿ����� �� �̻� ĳ������ �ʴ´�.
	if ( (m_lCacheSize / 1024 / 1024) > ZOption::GetInstance().GetMaxCacheMemoryMB() )
	{
		return false;
	}

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_numImageVectorSize ) iIndex = (int)m_numImageVectorSize - 1;

	// �̹� ĳ�õǾ� �ִ��� ã�´�.
	bool bFound = false;
	tstring strFileName;

	strFileName = m_imageMap[iIndex];

	if ( strFileName.length() <= 0 ) return false;

	{
		CLockObjUtil lock(m_cacheLock);
		if ( m_cacheData.count(strFileName) > 0)
		{
			// found!
			bFound = true;
		}
	}

	if ( bFound == false )	// ĳ�õǾ� ���� ������ �о���δ�.
	{
		ZImage cacheReayImage;
		if ( cacheReayImage.LoadFromFile(strFileName) )
		{
			/*
			{/// Debug Code
				//if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.

				AddCacheData(strFileName, temp);

				if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
				*/

				/*
				WORD tempWidth, tempHeight;
				long tempImageSize = cacheReayImage.GetImageSize();

				tempWidth = cacheReayImage.GetWidth();
				tempHeight = cacheReayImage.GetHeight();

				temp.Resize(10, 10);

				tempWidth = cacheReayImage.GetWidth();
				tempHeight = cacheReayImage.GetHeight();
				tempImageSize = cacheReayImage.GetImageSize();
			}
			*/

#if 0
			if ( ZOption::GetInstance().IsBigToSmallStretchImage() )
			{
				RECT screenRect = { 0 };
				if ( false == ZMain::GetInstance().getCurrentScreenRect(screenRect) ) return false;

				RECT imageRect = { 0 };
				imageRect.right = cacheReayImage.GetWidth();
				imageRect.bottom = cacheReayImage.GetHeight();

				if ( imageRect.right > screenRect.right || imageRect.bottom > screenRect.bottom )
				{
					RECT newRect = GetResizedRectForBigToSmall(screenRect, imageRect);

					if ( newRect.right != imageRect.right || newRect.bottom != imageRect.bottom )
					{
						DebugPrintf(TEXT("Resizing Cache..."));
						cacheReayImage.Resize((WORD)newRect.right, (WORD)newRect.bottom);
					}
				}
			}

			if ( ZOption::GetInstance().IsSmallToBigStretchImage() )
			{
				RECT screenRect;
				if ( false == ZMain::GetInstance().getCurrentScreenRect(screenRect) ) return false;

				/*
				if ( screenRect.right > 10 )
				{
					--screenRect.right;
				}
				if ( screenRect.bottom > 10 )
				{
					--screenRect.bottom;
				}
				*/

				RECT imageRect = { 0 };
				imageRect.right = cacheReayImage.GetWidth();
				imageRect.bottom = cacheReayImage.GetHeight();

				if ( imageRect.right < screenRect.right && imageRect.bottom < screenRect.bottom )
				{
					RECT newRect = GetResizedRectForSmallToBig(screenRect, imageRect);

					if ( newRect.right != imageRect.right || newRect.bottom != imageRect.bottom )
					{
						DebugPrintf(TEXT("Resizing Cache..."));
						cacheReayImage.Resize((WORD)newRect.right, (WORD)newRect.bottom);
					}
				}
			}
#endif
			// ���� �̹����� ���� ������ ������
			if ( (m_lCacheSize + cacheReayImage.GetImageSize()) / 1024 / 1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
			{
				int iTemp = 100;
				int iFarthestIndex = -1;

				do
				{
					// ĳ�õǾ� �ִ� �͵� �� ���� ���� index ���� �հ��� ã�´�.
					iFarthestIndex = _GetFarthestIndexFromCurrentIndex();
					_ASSERTE(iFarthestIndex >= 0 );

					size_t nCachedFarthestDiff = abs(iFarthestIndex - m_iCurrentIndex);
					size_t nToCacheDiff = abs(iIndex - m_iCurrentIndex );

					if ( nCachedFarthestDiff < nToCacheDiff )
					{
						// ĳ�� �ߴ� �� �� ���� �ָ��ִ� ���� �̹��ź��� ���� ������ ���̻� ĳ������ �ʴ´�
						return false;
					}
					else if ( nCachedFarthestDiff == nToCacheDiff )
					{
						// ĳ���ߴ� �Ŷ� �̹��� ĳ���� ���� ������ ��ġ�� ������

						if ( m_lastActionDirection == eLastActionDirection_FORWARD )
						{
							// ������ ���� ���̸� ���� �ָ��ִ� ���� prev �̸� �����(������ ���� ���� ���� next image �� �켱������ ����)
							if ( iFarthestIndex >= iIndex )
							{
								// ĳ�õǾ� �ִ� ���� ����� �ʴ´�.
								return false;
							}
						}
						else
						{
							// �ڷ� ���� ���̸� ���� �ָ��ִ� ���� next �̸� �����.
							if ( iFarthestIndex <= iIndex )
							{
								return false;
							}
						}
					}

					//  ���� ���� �� ������ ������ ���� �� ���� Ŭ�����ϰ�, ���� ���� ĳ���� ���� ���� ���� ��Ȳ�̴�.

					// ���� �� ���� clear �Ѵ�.
					{
						CLockObjUtil lock(m_cacheLock);

						tstring strFindFileName = m_imageMap[iFarthestIndex];
						CacheMapIterator it = m_cacheData.find(m_imageMap[iFarthestIndex]);

						if ( it != m_cacheData.end() )
						{
							m_lCacheSize -= it->second.GetImageSize();
							m_cacheData.erase(it);

							DebugPrintf(TEXT("Farthest one clear"));
						}
						else
						{
							_ASSERTE(!"Can't find the cache data.");
							return false;
						}
					}

					// ���� ��� ���� �뷮�� Ȯ�������� �ٽ� �� �̹����� ���� �� �ִ� �� ĳ�ø� üũ�Ѵ�.
					if ( (m_lCacheSize + cacheReayImage.GetImageSize()) / 1024 / 1024 <= ZOption::GetInstance().GetMaxCacheMemoryMB() )
					{
						AddCacheData(strFileName, cacheReayImage);
						return true;
					}

					// ������ ���ѷ����� �����ϱ� ���� 100���� ������.
					--iTemp;
				} while( iTemp > 0 );

				_ASSERTE(iTemp >= 0 );
			}
			else
			{
				AddCacheData(strFileName, cacheReayImage);
				if ( m_bNewChange) return false;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.
			}
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

	while ( m_bGoOn ) // thread loop
	{
		m_bNowCaching = true;
		iPos = 0;
		_ASSERTE((int)m_numImageVectorSize == (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_imageMap.size());
		_ASSERTE(m_iCurrentIndex <= (int)m_numImageVectorSize);

#ifdef _DEBUG

		{
			CLockObjUtil lock(m_cacheLock);
			if ( m_cacheData.empty() )
			{
				_ASSERTE(m_lCacheSize == 0);
			}
		}
#endif

		for ( i=0; i<ZOption::GetInstance().m_iMaxCacheImageNum/2; ++i)
		{
			if ( m_bNewChange) break;	// ���纸�� �ִ� ���� �ε����� �ٲ������ ���� ���� for �� �����Ѵ�.

			/// ���纸�� �ִ� ���⿡ ���� ����� �̹����� ���� ĳ���� ������ �Ǵ��Ѵ�.
			if ( m_lastActionDirection == eLastActionDirection_FORWARD )
			{
				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;

				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;
			}
			else if ( m_lastActionDirection == eLastActionDirection_BACKWARD )
			{
				// left side
				if ( false == _CacheIndex(m_iCurrentIndex - iPos) ) break;

				// right side
				if ( false == _CacheIndex(m_iCurrentIndex + iPos) ) break;
			}
			else
			{
				_ASSERTE(false);
			}

			++iPos;
		}

		//DebugPrintf("wait event");

		m_bNowCaching = false;
		m_hCacheEvent.wait();
		m_bNewChange = false;

		DebugPrintf(TEXT("Recv event"));
	}
}

bool ZCacheImage::hasCachedData(const tstring & strFilename, int iIndex)
{
	if ( false == ZOption::GetInstance().IsUseCache() ) return false;

	// index �� üũ�Ѵ�.
	m_iCurrentIndex = iIndex;
	m_bNewChange = true;

	m_hCacheEvent.setEvent();

	{
		CLockObjUtil lock(m_cacheLock);

		if ( m_cacheData.count(strFilename) > 0 ) return true;
	}
	return false;
}

void ZCacheImage::getCachedData(const tstring & strFilename, ZImage & image)
{
	CacheMapIterator it;
	CLockObjUtil lock(m_cacheLock);

	it = m_cacheData.find(strFilename);
	if ( it == m_cacheData.end() )
	{
		_ASSERTE(!"Can't get NOT CACHED");
	}
	else
	{
		DebugPrintf(TEXT("Cache hit"));
	}
	image = it->second;
}


void ZCacheImage::AddCacheData(const tstring & strFilename, ZImage & image)
{
	CLockObjUtil lock(m_cacheLock);

	/// �̹� ĳ�õǾ� ������ ĳ������ �ʴ´�.
	if ( m_cacheData.count(strFilename) > 0) return;

	/// �뷮�� üũ�ؼ� �� �̹����� ĳ������ �� ������ �Ѿ���� ĳ������ �ʴ´�.
	if ( (m_lCacheSize + image.GetImageSize()) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() ) return;

	DebugPrintf(TEXT("%s added to cache"), strFilename.c_str());

#ifdef _DEBUG
	m_cacheData[strFilename] = image;
	m_lCacheSize += m_cacheData[strFilename].GetImageSize();
#else
	try
	{
		m_cacheData[strFilename] = image;
	}
	catch ( ... )
	{
		return;
	}
	m_lCacheSize += m_cacheData[strFilename].GetImageSize();
#endif
}


void ZCacheImage::debugShowCacheInfo()
{
	RECT rt;
	if ( false == ZMain::GetInstance().getCurrentScreenRect(rt) )
	{
		DebugPrintf(TEXT("CurrentScreenSize : Cannot getCurrentScreenRect"));
		return;
	}
	DebugPrintf(TEXT("CurrentScreenSize : %d, %d"), rt.right, rt.bottom);

	std::map < tstring, ZImage >::iterator it;

	CLockObjUtil lock(m_cacheLock);
	for ( it = m_cacheData.begin(); it != m_cacheData.end(); ++it )
	{
		ZImage & image = it->second;

		DebugPrintf(TEXT("[%s] width(%d) height(%d)"), it->first.c_str(), image.GetWidth(), image.GetHeight());
	}
}


void ZCacheImage::clearCache()
{
	CLockObjUtil lock(m_cacheLock);
	m_cacheData.clear();
	m_lCacheSize = 0;
	DebugPrintf(TEXT("Clear cache data"));
}


long ZCacheImage::GetCachedKByte() const
{
	return (m_lCacheSize/1024);
}
