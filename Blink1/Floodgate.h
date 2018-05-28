#ifndef _FLOODGATE_H_
#define _FLOODGATE_H_

#include <semaphore.h>

//не используется

namespace Threading {

	class Floodgate {
		sem_t sem;
		bool work;

	public:
		Floodgate(bool init_state) {
			sem_init(&sem, 0, 0);
			work = init_state;
		}
		~Floodgate() {
			sem_destroy(&sem);
		}

		bool TrySwoosh() {
			if (work) {
				return false;
			}
			return sem_wait(&sem);
		}

		void Lock() {
			work = false;
		}

		void Unlock() {
			work = true;
			// :(
			try {
				sem_post(&sem);
			}
			catch (...) {}
		}

		bool State() {
			return work;
		}
	};
}

#endif //_FLOODGATE_H_
