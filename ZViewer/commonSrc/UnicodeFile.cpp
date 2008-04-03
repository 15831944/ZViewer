
#include "stdafx.h"
#include "UnicodeFile.h"

/// ������ ����.
bool CUnicodeFile::open(const tstring & strFileName, eFileOpenMode openMode)
{
	char filename[FILENAME_MAX] = { 0 };

#ifdef _UNICODE
	WideCharToMultiByte(CP_THREAD_ACP, 0, strFileName.c_str(), (int)strFileName.size(), filename, 256, NULL, NULL);
#else
	StringCchPrintf(filename, FILENAME_MAX, strFileName.c_str());
#endif

	if ( openMode == eFileOpenMode_READ )
	{
		m_inputStream.open(filename, std::ios_base::binary);

		if ( m_inputStream.is_open() == false )
		{
			return false;
		}

		m_inputStream.seekg(-1, std::ios_base::end);

		size_t pos = m_inputStream.tellg();
		if ( pos < 2 || pos % 2 != 1)
		{
			_ASSERTE(!"not unicode file");
			m_inputStream.close();
			return false;
		}
		m_inputStream.seekg(0, std::ios_base::beg);

		wchar_t boom;
		m_inputStream.read((char*)&boom, sizeof(boom));

		switch ( boom )
		{
		case eBoomCode_LITTLEENDIAN:
			m_bBigEndian = false;
			break;

		case eBoomCode_BIGENDIAN:
			m_bBigEndian = true;
			break;

		default:
			m_inputStream.close();
			return false;
		}
	}
	else if ( openMode == eFileOpenMode_WRITE )
	{
		m_outputStream.open(filename, std::ios_base::binary);

		wchar_t boom = eBoomCode_LITTLEENDIAN;
		m_outputStream.write((char*)&boom, sizeof(boom));

		m_bBigEndian = false;
	}

	return true;
}


bool CUnicodeFile::getLine(std::wstring & str)
{
	// ��Ƽ����Ʈ ���ۿ� ���پ� ��� ��ƾ� �Ѵ�.
	str.resize(0);
	wchar_t aCh = L'\0';

	if ( m_inputStream.eof() )
	{
		return false;
	}

	while( m_inputStream.eof() == false )
	{
		m_inputStream.read((char*)&aCh,2);

		/// ���̻� �� �о����� �� ���� ���� ���̴�.
		if ( -1 == m_inputStream.tellg() ) break;

		if( m_bBigEndian )
		{
			// �򿣵���̶�� ��Ʋ��������� ��ȯ�Ѵ�.
			// ���ڰ迭�� ��Ʋ������̱⶧���̴�.
			aCh = _changeEndian(aCh);
		}

		if (EOF == aCh )// ������ ���ΰ�� 
		{
			if(str.empty())
			{
				return false;
			}		

			break;
		}

		/// end line �� ������ �� ���� ���� ���̴�.
		if( L'\n' == aCh)
		{
			break;
		}
		else if( L'\r' == aCh )
		{
			continue;
		}

		str += aCh;
	}

	return true;
}


bool CUnicodeFile::getLine(std::string & str)
{
	if ( m_inputStream.eof() ) return false;

	char buff[256];
	m_inputStream.getline(buff, 256);

	return true;
}