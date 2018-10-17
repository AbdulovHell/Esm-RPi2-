#ifndef _MAIN_H_
#define _MAIN_H_

#include "Task.h"

void InitShutdown();

namespace Threading {
	extern std::vector<Threading::Task*> MainTasks;
	extern std::mutex* TasksMutex;

	void AddTask(Threading::Task* tsk);
}
#endif //_MAIN_H_