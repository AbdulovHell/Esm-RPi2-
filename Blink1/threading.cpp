#include "sys_headers.h"

#include "threading.h"

Threading::Thread::Thread(void*(func)(void*), void* arg)
{
	pthread_create(&threadHandle, NULL, func, arg);
}

Threading::Thread::~Thread()
{
	pthread_cancel(threadHandle);
}

pthread_t Threading::Thread::GetThrdHandle()
{
	return threadHandle;
}

int Threading::Thread::Join()
{
	return pthread_join(threadHandle, NULL);
}
