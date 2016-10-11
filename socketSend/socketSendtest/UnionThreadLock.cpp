#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include "UnionThreadLock.h"

#ifdef WIN32

int	UnionMutexLock(CTCriticalSection	*mutex)
{
	mutex->Lock();
	return 0;
}

int	UnionMutexUnLock(CTCriticalSection  *mutex)
{
	mutex->Unlock();
	return 0;
}

int UnionGetThreadID()
{
	return GetCurrentThreadId();
}

#else
int	UnionMutexLock(pthread_mutex_t	*mutex)
{
	int	ret=0;
	ret = pthread_mutex_lock(mutex);
	return ret;
}

int	UnionMutexUnLock(pthread_mutex_t  *mutex)
{
	int	ret;
	ret = pthread_mutex_unlock(mutex);
	return ret;
}

int UnionGetThreadID()
{
	return pthread_self();
}

#endif

