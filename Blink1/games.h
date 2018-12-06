#ifndef _GAMES_H_
#define _GAMES_H_

namespace Games {
	using namespace std;

	void LaunchSecretMenu(Display::Display * disp,uint32_t param);
	void Selfdestruction(Display::Display * disp, uint32_t param);
	void SnakeMenu(Display::Display * disp, uint32_t param);
	int SnakeGame(Display::Display * disp, bool isBordless, int speed_ms,int mode);
	void RacingMenu(Display::Display* disp, uint32_t param);
	int RacingGame2(Display::Display * disp, int speed);
	int RacingGame4(Display::Display * disp, int speed);
	void PlaceCars(char** tracks);
	void TetrisGame(Display::Display * disp, uint32_t param);
}

#endif //_GAMES_H_
