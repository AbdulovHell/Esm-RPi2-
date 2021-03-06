#ifndef _MAIN_H_
#define _MAIN_H_

#include "Task.h"

void InitShutdown();

namespace Threading {
	extern std::vector<std::unique_ptr<Threading::Task>> MainTasks;
	extern std::mutex* TasksMutex;

	void AddTask(std::unique_ptr<Threading::Task> tsk);
}
#endif //_MAIN_H_