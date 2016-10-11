#include <sys/stat.h>
#include <stdarg.h>
#include <windows.h>
#include "UnionThreadLock.h"



DWORD gdwStart, gdwStop ;


#define MAX_THREAD_NUM  2000
HANDLE  hThreadID[MAX_THREAD_NUM];
unsigned int iThreadID[MAX_THREAD_NUM];// 记录线程ID号 
#define HSMCMD 1


int gthreadNum = 1;   //线程数
long testRecordNum = 0;
long total = 0;
long recordNo = 0;
long loops = 1;
long gSuccNum = 0;
long gFailedNum = 0;
int gRandLen = 32;
int gEccIndex = 1;

void UnionInitCountFun()
{
	gSuccNum = 0;
	gFailedNum = 0;
	total = 0;
	recordNo = 0;
}

typedef unsigned int(*PTHREADFUNC)(void*);
PTHREADFUNC threadfunc;


int creatThread(int threadnum,int type)
{
	int i = 0;

	UnionInitCountFun();
	for(i=0;i<threadnum&&i<MAX_THREAD_NUM;i++)
	{
		switch(type)
		{		
		case HSMCMD:
			hThreadID[i] = (HANDLE)_beginthreadex(NULL, 0, threadfunc, NULL, 0, &iThreadID[i]);
			break;
		default:
			break;
		}
	}

	for(i=0;i<threadnum&&i<MAX_THREAD_NUM;i++) 
	{
		WaitForSingleObject(hThreadID[i],INFINITE);   //等待线程结束
	}

	return i;
}





