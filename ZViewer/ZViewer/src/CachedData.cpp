/* ------------------------------------------------------------------------
 *
 * CachedData.cpp
 *
 * 2008.12.13 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"

#include "CachedData.h"
#include "ZOption.h"

void CachedData::ClearCachedImageData()
{
	CacheMapIterator it;

	CLockObjUtil lock(m_cacheLock);
	for ( ; ; )
	{
		if ( m_cacheData.empty() ) break;
		it = m_cacheData.begin();
		if ( it->second )
		{
			delete it->second;
		}
		m_cacheData.erase(it);
	}

	m_cacheData.clear();

	m_lCacheSize = 0;
}

void CachedData::ShowCacheInfo() const
{
	CacheMapIterator_const it;

	CLockObjUtil lock(m_cacheLock);
	for ( it = m_cacheData.begin(); it != m_cacheData.end(); ++it )
	{
		const ZImage * pImage = it->second;

		DebugPrintf(TEXT("[%s] %dbyte"), it->first.c_str(), pImage->GetImageSize());
	}
}

bool CachedData::HasCachedData(const int index) const
{
	CLockObjUtil lock(m_cacheLock);

	/// �ε����� ���ϸ����� �ٲ۴�.
	std::map < int, tstring >::const_iterator it = m_imageIndex2FilenameMap.find(index);
	if ( it != m_imageIndex2FilenameMap.end() )
	{
		const tstring & strNextFilename = it->second;
		if ( true == HasCachedData(strNextFilename) )
		{
			DebugPrintf(TEXT("���� ���� ĳ����"));
			return true;
		}
	}
	else
	{
		assert(false);
	}
	return false;
}

void CachedData::PrintCachedData() const
{
	CLockObjUtil lock(m_cacheLock);

	CacheMapIterator_const it, endIt = m_cacheData.end();

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		DebugPrintf(TEXT("Cacaed Filename : %s"), it->first.c_str());
	}
}

/// ĳ�õǾ� �ִ� �����͵� �� ���� �ε����κ��� ���� �ָ��ִ� �ε����� ��´�.
int CachedData::GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex)
{
	CLockObjUtil lock(m_cacheLock);
	assert(m_imageFilename2IndexMap.size() == m_imageIndex2FilenameMap.size());
	CacheMapIterator it, endIt = m_cacheData.end();

	int iFarthestIndex = iCurrentIndex;
	int iDistanceMax = 0;
	int iDistance = 0;
	int iTempIndex = 0;

	tstring strFarthest;

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		iTempIndex = m_imageFilename2IndexMap[it->first];
		iDistance = abs(iTempIndex - iCurrentIndex);

		if ( iDistance > iDistanceMax )
		{
			iDistanceMax = iDistance;
			iFarthestIndex = iTempIndex;
			strFarthest = it->first;
		}
	}

	assert(iFarthestIndex >= 0 );

	return iFarthestIndex;
}

void CachedData::SetNewFileList(const std::vector < FileData > & v)
{
	CLockObjUtil lock(m_cacheLock);

	m_imageIndex2FilenameMap.clear();
	m_imageFilename2IndexMap.clear();
	for ( size_t i = 0; i < v.size(); ++i)
	{
		m_imageIndex2FilenameMap.insert(std::make_pair((int)i, v[i].m_strFileName));
		m_imageFilename2IndexMap.insert(std::make_pair(v[i].m_strFileName, (int)i));
	}

	DebugPrintf(TEXT("imageIndex2FilenameMapSize : %d"), m_imageIndex2FilenameMap.size());
}

bool CachedData::GetCachedData(const tstring & strFilename, ZImage * & pImage) const
{
	CLockObjUtil lock(m_cacheLock);

	CacheMapIterator_const it;

	it = m_cacheData.find(strFilename);
	if ( it == m_cacheData.end() )
	{
		assert(!"Can't get NOT CACHED");
		return false;
	}
	else
	{
		DebugPrintf(TEXT("Cache hit"));
	}

	TIMECHECK_START("GetCacheData");
	pImage = it->second;
	TIMECHECK_END();

	return true;
}

bool CachedData::ClearFarthestDataFromCurrent(const int iFarthestIndex)
{
	CLockObjUtil lock(m_cacheLock);

	std::map < int , tstring >::iterator itFileName = m_imageIndex2FilenameMap.find(iFarthestIndex);
	if ( itFileName == m_imageIndex2FilenameMap.end() )
	{
		assert(!"Can't find the cache data.");
		return false;
	}

	CacheMapIterator it = m_cacheData.find(itFileName->second);

	if ( it != m_cacheData.end() )
	{
		if ( NULL == it->second )
		{
			assert(it->second);
			return false;
		}
		m_lCacheSize -= it->second->GetImageSize();
		
		delete (it->second);
		m_cacheData.erase(it);

		DebugPrintf(TEXT("Farthest one clear"));
	}
	else
	{
		assert(!"Can't find the cache data.");
		return false;
	}
	return true;
}


void CachedData::InsertData(const tstring & strFilename, ZImage * pImage, bool bForceCache)
{
	/// �̹� ĳ�õǾ� ������ ĳ������ �ʴ´�.
	if ( HasCachedData(strFilename) )
	{
		assert(false);
		delete pImage;
		pImage = NULL;
		return;
	}

	if ( false == bForceCache )
	{
		/// �뷮�� üũ�ؼ� �� �̹����� ĳ������ �� ������ �Ѿ���� ĳ������ �ʴ´�.
		if ( (GetCachedTotalSize() + pImage->GetImageSize()) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
		{
			DebugPrintf(_T("-- �� �̹����� ĳ���ϸ� �뷮������ �Ѿ ĳ������ �ʽ��ϴ� -- : %s"), strFilename.c_str());
			delete pImage;
			return;
		}
	}

#ifndef _DEBUG
	try
	{
#endif
		DWORD dwStart = GetTickCount();

		CLockObjUtil lock(m_cacheLock);
		{
			if ( m_cacheData.find(strFilename) != m_cacheData.end() )
			{
				assert(!"--- �̹� ĳ�ÿ� �ִ� ������ �ٽ� �������� �մϴ� ---");
				delete pImage;
				return;
			}
			m_cacheData[strFilename] = pImage;
		}
		m_lCacheSize += pImage->GetImageSize();

		DebugPrintf(TEXT("%s added to cache"), strFilename.c_str());
		DWORD dwEnd = GetTickCount();
		DebugPrintf(TEXT("Cache insert time : %d filename(%s)"), dwEnd - dwStart, strFilename.c_str());

#ifndef _DEBUG
	}
	catch ( ... )
	{
		return;
	}
#endif

}
