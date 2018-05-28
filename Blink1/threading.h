#ifndef _THREADING_H_
#define _THREADING_H_

namespace Threading {
	using namespace std;	

	class Thread {
	protected:
		pthread_t threadHandle;
		struct RTData {
			bool* bStopFlag;
			int iSocket;
		};
	public:
		Thread() {}
		Thread(void*(func)(void*), void* arg);
		~Thread();
		pthread_t GetThrdHandle();
		int Join();
	};
}
#endif	//#ifndef _THREADING_H_
