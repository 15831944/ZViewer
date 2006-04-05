/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * ShortTimeMessage.h
 *
 * 2006. 3. 13 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

/// ��� �����ִ� �޽����� ���� �̱��� Ŭ����
class CShortTimeMessage
{
    CShortTimeMessage();

public:
    static CShortTimeMessage & getInstance();
    ~CShortTimeMessage();

	void setMessage(const std::string & strMsg, DWORD dwShowTimeInMilliSec);

	/// Ÿ�̸Ӱ� ȣ��Ǿ��� ��
	void onTimer();

protected:
	/// ������ �޽���
	std::string m_strMessage;
	
	/// ������ �޽����� ����� �ð�
	DWORD m_hideTick;
};
