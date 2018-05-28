#ifndef _GAMES_H_
#define _GAMES_H_

namespace Games {
	using namespace std;

	int LaunchSecretMenu(Display::Display * disp);
	void Selfdestruction(Display::Display * disp);
	void SnakeMenu(Display::Display * disp);
	int SnakeGame(Display::Display * disp, bool isBordless, int speed_ms);
	void RacingMenu(Display::Display* disp);
	int RacingGame2(Display::Display * disp, int speed);
	int RacingGame4(Display::Display * disp, int speed);
	void PlaceCars(char** tracks);
	int TetrisGame(Display::Display * disp);
}

#endif //_GAMES_H_
