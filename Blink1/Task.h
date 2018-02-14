#ifndef _TASK_H_
#define _TASK_H_

namespace Threading{

	enum TaskType : int {
		SetAttCh,
		Quit,
		Empty,
		KeyUp,
		KeyDown,
		KeyLeft,
		KeyRight,
		KeyMid,
		LaunchGame
	};

	class Task {
	public:
		Task() {}
		virtual TaskType GetType() = 0;
		virtual uint8_t Att() = 0;
		virtual uint8_t Ch() = 0;
	};

	class TaskSetAttCh : virtual public Task {
		uint8_t att;
		uint8_t ch;
	public:
		TaskSetAttCh(uint8_t _att, uint8_t _ch) { att = _att; ch = _ch; }

		uint8_t Att() { return att; }
		uint8_t Ch() { return ch; }
		TaskType GetType() { return TaskType::SetAttCh; }
	};

	class TaskQuit : virtual public Task {
	public:
		TaskQuit() {}

		uint8_t Att() { return 0; }
		uint8_t Ch() { return 0; }
		TaskType GetType() { return TaskType::Quit; }
	};

	class TaskKeyUp : virtual public Task {
	public:
		TaskKeyUp() {}

		uint8_t Att() { return 0; }
		uint8_t Ch() { return 0; }
		TaskType GetType() { return TaskType::KeyUp; }
	};

	class TaskKeyDown : virtual public Task {
	public:
		TaskKeyDown() {}

		uint8_t Att() { return 0; }
		uint8_t Ch() { return 0; }
		TaskType GetType() { return TaskType::KeyDown; }
	};

	class TaskKeyLeft : virtual public Task {
	public:
		TaskKeyLeft() {}

		uint8_t Att() { return 0; }
		uint8_t Ch() { return 0; }
		TaskType GetType() { return TaskType::KeyLeft; }
	};

	class TaskKeyRight : virtual public Task {
	public:
		TaskKeyRight() {}

		uint8_t Att() { return 0; }
		uint8_t Ch() { return 0; }
		TaskType GetType() { return TaskType::KeyRight; }
	};

	class TaskKeyMid : virtual public Task {
	public:
		TaskKeyMid() {}

		uint8_t Att() { return 0; }
		uint8_t Ch() { return 0; }
		TaskType GetType() { return TaskType::KeyMid; }
	};

	class TaskLaunchGame :virtual public Task {
	public:
		TaskLaunchGame() {}

		uint8_t Att() { return 0; }
		uint8_t Ch() { return 0; }
		TaskType GetType() { return TaskType::LaunchGame; }
	};
}

#endif