#ifndef _TASK_H_
#define _TASK_H_

namespace Threading{

	enum TaskType : int {
		SetAttCh,
		Quit,
		Empty
	};

	//Родитель
	class Task {
	public:
		Task() {}
		virtual TaskType GetType() = 0;
		virtual void Run() = 0;
	};

	//Потомки
	class TaskSetAttCh : virtual public Task {
		uint8_t att;
		uint8_t ch;
	public:
		TaskSetAttCh(uint8_t _att, uint8_t _ch) { att = _att; ch = _ch; }

		TaskType GetType() { return TaskType::SetAttCh; }
		void Run();
	};

	class TaskQuit : virtual public Task {
	public:
		TaskQuit() {}

		TaskType GetType() { return TaskType::Quit; }
		void Run();
	};

}

#endif