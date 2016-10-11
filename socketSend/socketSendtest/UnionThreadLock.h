#ifndef _UnionThreadLock
#define _UnionThreadLock
#ifndef WIN32
#include <pthread.h>
int	UnionMutexLock(pthread_mutex_t *mutex);
int	UnionMutexUnLock(pthread_mutex_t *mutex);

#else
#include <windows.h>
#include <process.h>
#include <time.h>

class CTCriticalSection
{
private:
	CRITICAL_SECTION CriticalSection;
	int				 locked;	// 锁状态
	time_t			 lockedTime;   // 最近加锁的时间
	
public:
	CTCriticalSection() { ::InitializeCriticalSection( &CriticalSection ); locked = 0;}
	~CTCriticalSection() { ::DeleteCriticalSection( &CriticalSection ); }
	
	void Enter() { ::EnterCriticalSection( &CriticalSection ); locked = 1; time(&lockedTime); }
	void Leave() { ::LeaveCriticalSection( &CriticalSection ); locked = 0; }
	void Lock() { ::EnterCriticalSection( &CriticalSection ); locked = 1; time(&lockedTime); }
	void Unlock() { ::LeaveCriticalSection( &CriticalSection ); locked = 0; }
	void DeleteLock () { ::DeleteCriticalSection( &CriticalSection ); locked = 0; }
	
	int	 IsLocked() { return locked; }   // 是否已加锁
	time_t	GetLockedTime( ) { return lockedTime; }  // 取最近加锁的时间
};

int	UnionMutexLock(CTCriticalSection *mutex);
int	UnionMutexUnLock(CTCriticalSection *mutex);


#endif

int UnionGetThreadID();

#endif
