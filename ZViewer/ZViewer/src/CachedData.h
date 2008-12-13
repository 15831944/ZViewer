/* ------------------------------------------------------------------------
 *
 * CachdeData.h
 *
 * 2008.12.13 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include <map>

#include "../../commonSrc/ZImage.h"

typedef std::map < tstring, ZImage >::iterator CacheMapIterator;
typedef std::map < tstring, ZImage >::const_iterator CacheMapIterator_const;


class CachedData
{
public:
	CachedData() {}
	virtual ~CachedData(){}

	void Clear() { m_cacheData.clear(); }
	size_t Size() const { return m_cacheData.size(); }

	void PrintCachedData() const;

	bool IsEmpty() const
	{
		if ( m_cacheData.empty() ) return true;
		return false;
	}

	void InsertData(const tstring & strFilename, ZImage & image)
	{
		m_cacheData[strFilename] = image;
	}

	void ShowCacheInfo() const;

	bool HasCachedData(const int index) const;

	bool HasCachedData(const tstring & strFilename) const
	{
		return (m_cacheData.count(strFilename) > 0);
	}

	size_t GetIndex2FilenameMapSize() const { return m_imageIndex2FilenameMap.size(); }
	void SetNewFileList(const std::vector < FileData > & v, const size_t & numImageVectorSize);

	bool GetCachedData(const tstring & strFilename, ZImage & image) const;
	bool ClearFarthestDataFromCurrent(const int iFarthestIndex, long & cacheSize);

	/// ĳ�õǾ� �ִ� �����͵� �� ���� �ε����κ��� ���� �ָ��ִ� �ε����� ��´�.
	int GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex);

	tstring GetFilenameFromIndex(const int iIndex) { return m_imageIndex2FilenameMap[iIndex]; }

protected:
	std::map < tstring, ZImage > m_cacheData;		///< ������ ĳ���� �����͸� ������ �ִ� ��
	std::map < int , tstring > m_imageIndex2FilenameMap;	///< �̹��� ������ �ε��� ��ȣ,�����̸� ��
	std::map < tstring, int > m_imageFilename2IndexMap;		///< �̹��� �����̸�,�ε��� ��ȣ ��

private:
	/// ���� �����ڿ� = ������ ����
	CachedData & operator=(const CachedData &);
	CachedData(const CachedData &);
};

