#include "cg.h"

namespace Stuff {
	struct termios tOld, tNew;
}

void Stuff::Conio::initTermios(int echo)
{
	tcgetattr(0, &tOld); /* grab old terminal i/o settings */
	tNew = tOld; /* make new settings same as old settings */
	tNew.c_lflag &= ~ICANON; /* disable buffered i/o */
	tNew.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, &tNew); /* use these new terminal i/o settings now */
}

void Stuff::Conio::resetTermios(void)
{
	tcsetattr(0, TCSANOW, &tOld);
}

char Stuff::Conio::getch_(int echo)
{
	char ch;
	initTermios(echo);
	ch = (char)getchar();
	resetTermios();
	return ch;
}

char Stuff::Conio::getch(void)
{
	return getch_(0);
}

char Stuff::Conio::getche(void)
{
	return getch_(1);
}
