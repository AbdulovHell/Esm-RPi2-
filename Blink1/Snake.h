#ifndef _SNAKE_H_
#define _SNAKE_H_

#include <vector>

namespace Stuff {
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
			Tail[0].x = Head.x;
			Tail[0].y = Head.y;
			Head.x = x;
			Head.y = y;
			for (size_t i = 1; i < Tail.size(); i++) {
				Tail[i] = Tail[i - 1];
			}
		}

		void MoveAndGrow(int x, int y) {
			Tail[0].x = Head.x;
			Tail[0].y = Head.y;
			Head.x = x;
			Head.y = y;
			SCoord last;
			last.x = Tail[Tail.size() - 1].x;
			last.y = Tail[Tail.size() - 1].y;
			for (size_t i = 1; i < Tail.size(); i++) {
				Tail[i] = Tail[i - 1];
			}
			Tail.push_back(last);
		}

		SCoord operator[] (int index) {
			if (!index) {
				return Head;
			}
			else {
				return Tail[index-1];
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