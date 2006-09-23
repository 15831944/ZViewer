
#pragma once

#include <windows.h>
#include <fstream>
#include <string>

class CUnicodeFile
{
public:

	enum eFileOpenMode
	{
		eFileOpenMode_READ,
		eFileOpenMode_WRITE,
	};

	enum eBoomCode
	{
		eBoomCode_BIGENDIAN    = 0xfffe,
		eBoomCode_LITTLEENDIAN = 0xfeff,
	};

	CUnicodeFile()
	{

	}

	~CUnicodeFile()
	{

	}

	/// ������ ����.
	bool open(const tstring & strFileName, eFileOpenMode openMode);

	/// ������ ������ ���� ���´�. \r, \n �� �������� �ʴ´�.
	bool getLine(std::wstring & str);

	bool getLine(std::string & str);

	void write(const std::wstring & str)
	{
		m_outputStream.write((char*)str.c_str(), (std::streamsize)(str.size() * sizeof(wchar_t)));
	}

	void write(const std::string & str)
	{
		m_outputStream.write((char*)str.c_str(), (std::streamsize)(str.size() * sizeof(char)));
	}

	void writeLine(const tstring & str)
	{
		write(str);
		write(TEXT("\r\n"));
	}

protected:

	/// ������� �����Ѵ�. �򿣵�� <-> ��Ʋ�����
	const wchar_t _changeEndian(const wchar_t src)
	{
		BYTE hiByte = HIBYTE(((WORD)src));
		BYTE loByte = LOBYTE(((WORD)src));

		return  (wchar_t)((((WORD)(loByte)) << 8) | ((WORD)(hiByte)));
	}

	std::ifstream m_inputStream;		///< ���Ϸκ��� �б� ���� ��Ʈ��
	std::ofstream m_outputStream;		///< ���Ͽ� ���� ���� ��Ʈ��

	eFileOpenMode m_openMode;

	bool m_bBigEndian;
};