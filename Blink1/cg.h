#ifndef _CG_H_
#define _CG_H_

#include <termios.h>
#include <stdio.h>

//реализация старой стандартной функции getch

namespace Stuff {
	
	extern struct termios tOld, tNew;

	class Conio {

		/* Initialize new terminal i/o settings */
		static void initTermios(int echo);

		/* Restore old terminal i/o settings */
		static void resetTermios(void);

		/* Read 1 character - echo defines echo mode */
		static char getch_(int echo);
	public:
		/* Read 1 character without echo */
		static char getch(void);

		/* Read 1 character with echo */
		static char getche(void);

		/* Let's test it out */
		/*int main(void) {
			char c;
			printf("(getche example) please type a letter: ");
			c = getche();
			printf("\nYou typed: %c\n", c);
			printf("(getch example) please type a letter...");
			c = getch();
			printf("\nYou typed: %c\n", c);
			return 0;
		}*/
	};
}
#endif