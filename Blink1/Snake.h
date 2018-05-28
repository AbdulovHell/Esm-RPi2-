#ifndef _SNAKE_H_
#define _SNAKE_H_

#include <vector>

namespace Games {
	struct SCoord {
		int x;
		int y;

		SCoord operator= (SCoord &other) {
			x = other.x;
			y = other.y;
			return *this;
		}
	};

	class Snake {
		SCoord Head;
		std::vector<SCoord> Tail;
	public:
		Snake(SCoord* head, SCoord* bodypart) {
			Head = *head;
			Tail.push_back(*bodypart);
		}

		void Move(int x, int y) {
			for (size_t i = Tail.size() - 1; i > 0; i--) {
				Tail[i] = Tail[i - 1];
			}

			Tail[0].x = Head.x;
			Tail[0].y = Head.y;
			Head.x = x;
			Head.y = y;
		}

		void MoveAndGrow(int x, int y) {
			SCoord last;
			Tail.push_back(last);

			for (size_t i = Tail.size() - 1; i > 0; i--) {
				Tail[i] = Tail[i - 1];
			}

			Tail[0].x = Head.x;
			Tail[0].y = Head.y;
			Head.x = x;
			Head.y = y;
		}

		SCoord operator[] (int index) {
			if (!index) {
				return Head;
			}
			else {
				return Tail[index - 1];
			}
		}

		SCoord Part(int index) {
			if (!index) {
				return Head;
			}
			else {
				return Tail[index - 1];
			}
		}
	};
}

#endif