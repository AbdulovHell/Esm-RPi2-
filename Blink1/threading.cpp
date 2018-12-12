#include "sys_headers.h"

#include "threading.h"
#include "settings.h"
#include "Task.h"
#include "main.h"
#include "syslogs.h"
#include "stuff.h"

namespace Threading {
	float Temperature;
	bool UpdateNow;
}

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

void * Threading::TimingThread::Timing(void * ptr_null)
{
	time_t LastTime, CurTime;
	time(&CurTime);
	LastTime = CurTime;
	Threading::SALog.Append("Start timing");
	while (1) {
		time(&CurTime);
		time_t Elapsed = CurTime - LastTime;
		if (Elapsed > 0) {
			//cout << CurTime << endl;
			Threading::SALog.Append("Set lifetime");
			Stuff::Storage->SetWorkTime(Stuff::Storage->GetWorkTime() + Elapsed);
			LastTime = CurTime;
			Threading::SALog.Append("Reques temperature");
			Threading::AddTask(Stuff::make_unique<Threading::TaskRequestTemp>(&Temperature));
			UpdateNow = true;
		}
		this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

Threading::TimingThread::~TimingThread()
{
	pthread_cancel(threadHandle);
}
