/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * ShortTimeMessage.cpp
 *
 * 2006. 3. 13 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "ShortTimeMessage.h"

CShortTimeMessage & CShortTimeMessage::getInstance()
{
    static CShortTimeMessage inst;
    return inst;
}


CShortTimeMessage::CShortTimeMessage()
{
}


CShortTimeMessage::~CShortTimeMessage()
{
}


void CShortTimeMessage::setMessage(const std::string & strMsg, DWORD dwShowTimeInMilliSec)
{
	m_strMessage = strMsg;
	m_hideTick = GetTickCount() - dwShowTimeInMilliSec;
}

/// Ÿ�̸Ӱ� ȣ��Ǿ��� ��
void CShortTimeMessage::onTimer()
{
	/// ������ �޽����� �ð��� ������ �ʾ����� ȭ�鿡 �׸���.
	if ( GetTickCount() < m_hideTick )
	{
		//ZMain::GetInstance().drawShortMessage(m_strMessage);
	}
	else /// ������ �ð��� �������� �׸��� �ʴ´�.............. ���� �޽����� ȭ�鿡 ������ �������Ѵ�..
	{
		//ZMain::GetInstance().drawShortMessage("");`
	}
}