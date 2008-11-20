/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * LockUtil.h
 *
 * 2006. 3. 11 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */


#pragma once

#include <windows.h>

/// �������� Event �� ���� ���� �ϴ� ��ƿ��Ƽ Ŭ����
class CEventObj
{
public:
	CEventObj()
	{
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	~CEventObj()
	{
		CloseHandle(m_hEvent);
	}

	bool setEvent()
	{
		return SetEvent(m_hEvent) == TRUE;
	}

	DWORD wait()
	{
		return WaitForSingleObject(m_hEvent, INFINITE);
	}

protected:
	/// ���������� ������ �ִ� �̺�Ʈ �ڵ�
	HANDLE m_hEvent;

};

/// �������� ũ��Ƽ�� ������ ���� ���� �ϴ� ��ƿ��Ƽ Ŭ����
class CLockObj
{
public:
	CLockObj()
	{
		InitializeCriticalSection(&m_cs);
	}

	~CLockObj()
	{
		DeleteCriticalSection(&m_cs);
	}

protected:
	/// ���������� ������ �ִ� ũ��Ƽ�� ����
	CRITICAL_SECTION m_cs;


private:

	friend class CLockObjUtil;

	inline void lock()
	{
		EnterCriticalSection(&m_cs);
	}

	inline void unlock()
	{
		LeaveCriticalSection(&m_cs);
	}


};

/// lock, unlock �� ���� �ϱ� ���� �����ִ� ��ƿ��Ƽ Ŭ����
class CLockObjUtil
{
public:
	CLockObjUtil(CLockObj & lockObj)
		: m_lockObj(lockObj)
	{
		lockObj.lock();
	}

	~CLockObjUtil()
	{
		m_lockObj.unlock();
	}

protected:
	/// �����ڿ��� ���� ũ��Ƽ�� ������ ���������� ������ �ִ� ��� ����
	CLockObj & m_lockObj;

	/// �Ҵ� �����ڰ� ���̴� �� �����ϴ� �ڵ�. �Ҵ� �����ڰ� ���̰� �Ǹ� ���� �߻��ϰ� ��
	CLockObjUtil & operator=(const CLockObjUtil & obj);
};

