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
	int				 locked;	// ��״̬
	time_t			 lockedTime;   // ���������ʱ��
	
public:
	CTCriticalSection() { ::InitializeCriticalSection( &CriticalSection ); locked = 0;}
	~CTCriticalSection() { ::DeleteCriticalSection( &CriticalSection ); }
	
	void Enter() { ::EnterCriticalSection( &CriticalSection ); locked = 1; time(&lockedTime); }
	void Leave() { ::LeaveCriticalSection( &CriticalSection ); locked = 0; }
	void Lock() { ::EnterCriticalSection( &CriticalSection ); locked = 1; time(&lockedTime); }
	void Unlock() { ::LeaveCriticalSection( &CriticalSection ); locked = 0; }
	void DeleteLock () { ::DeleteCriticalSection( &CriticalSection ); locked = 0; }
	
	int	 IsLocked() { return locked; }   // �Ƿ��Ѽ���
	time_t	GetLockedTime( ) { return lockedTime; }  // ȡ���������ʱ��
};

int	UnionMutexLock(CTCriticalSection *mutex);
int	UnionMutexUnLock(CTCriticalSection *mutex);


#endif

int UnionGetThreadID();

#endif
