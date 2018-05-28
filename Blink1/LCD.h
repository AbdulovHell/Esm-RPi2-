#ifndef _LCD_H_
#define _LCD_H_

#include "Display.h"

namespace Display {

	typedef unsigned char byte;
	
	extern const byte Height;
	extern const byte Width;

	class LCDScreen {
		Display* out_disp;
		byte** image;
		bool initHere = true;

		byte Sum(int row, int column_sta, int size);
	public:
		enum Size{
			Square_3x3_lb,
			Rectangle_2x3,
			Rectangle_2x4
		};

		LCDScreen(Display* disp);
		LCDScreen(Display* disp, byte fill);
		LCDScreen(Display* disp, byte** img);
		~LCDScreen();

		byte** GetImage() {
			return image;
		}

		void Draw(Size size);
		void UpdateMemory(Size size);
	};

	int Drop(byte** image, int x, int y);
	void SandTest(Display* disp);
}

#endif
