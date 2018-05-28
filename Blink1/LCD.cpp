#include "sys_headers.h"

#include "LCD.h"

namespace Display {

	//in pixels
	const byte Height = 3 * 8;
	const byte Width = 3 * 5;

	byte LCDScreen::Sum(int row, int column_sta, int size)
	{
		byte res = 0;

		for (int i = column_sta; i < column_sta + size; i++) {
			res += (byte)((image[row][i]) ? 1 : 0);
			if (i < column_sta + size - 1) res <<= 1;
		}

		return res;
	}
	LCDScreen::LCDScreen(Display * disp)
	{
		out_disp = disp;
		image = new byte*[Height];
		for (int i = 0; i < Height; i++) {
			image[i] = new byte[Width];
			for (int j = 0; j < Width; j++) {
				image[i][j] = (byte)(0);
			}
		}
	}
	LCDScreen::LCDScreen(Display * disp, byte fill)
	{
		out_disp = disp;
		image = new byte*[Height];
		for (int i = 0; i < Height; i++) {
			image[i] = new byte[Width];
			for (int j = 0; j < Width; j++) {
				image[i][j] = fill;
			}
		}
	}
	LCDScreen::LCDScreen(Display * disp, byte ** img)
	{
		out_disp = disp;
		image = img;
		initHere = false;
	}
	LCDScreen::~LCDScreen()
	{
		if (initHere) {
			for (int i = 0; i < Height; i++) {
				delete[] image[i];
			}
			delete[] image;
		}
	}
	void LCDScreen::Draw(Size size)
	{
		CustomSymbol s;
		int ordr = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				switch (size)
				{
				case Size::Square_3x3_lb:
					if (i == 0 && j == 2) continue;
					break;
				case Size::Rectangle_2x3:
					if (i == 0) continue;
					break;
				default:
					if (i == 0 && j == 2) continue;
					break;
				}
				for (int k = 0; k < 8; k++) {
					s.Rows[k] = Sum(k + j * 8, i * 5, 5);
				}
				out_disp->LoadSymbol(s, ordr);
				out_disp->SetCursorPos(j + 1, i + 7);	//отсчет от 1
				out_disp->SendText(ordr++);
			}
		}
	}
	void LCDScreen::UpdateMemory(Size size)
	{
		CustomSymbol s;
		int ordr = 0;
		int i, j,iMax,jMax;

		switch (size)
		{
		case Size::Square_3x3_lb:
			break;
		case Size::Rectangle_2x4:
			i = 0;
			iMax = 2;
			j = 0;
			jMax = 4;
			break;
		case Size::Rectangle_2x3:
		default:

			break;
		}

		for (i = 0; i < iMax; i++) {
			for (j = 0; j < jMax; j++) {
				for (int k = 0; k < 8; k++) {
					s.Rows[k] = Sum(k + j * 8, i * 5, 5);
				}
				out_disp->LoadSymbol(s, ordr++);
			}
		}
	}
	int Drop(byte ** image, int x, int y)
	{
		bool left = false, right = false;
		int sum = 0;
		int newx = x - 1;
		if (newx >= 0) {
			sum = image[y][newx] + image[y + 1][newx];
			if (sum == 0) {
				left = true;
			}
		}

		if (newx < Width) {
			newx = x + 1;
			sum = image[y][newx] + image[y + 1][newx];
			if (sum == 0) {
				right = true;
			}
		}

		if (left && right)
			return (random() % 2) ? -1 : 1;
		else if (left)
			return -1;
		else if (right)
			return 1;
		else
			return 0;
	}
	void SandTest(Display * disp)
	{

		byte** image = new byte*[Height];
		int skip = 0;
		for (int i = 0; i < Height; i++) {
			image[i] = new byte[Width];
			for (int j = 0; j < Width; j++) {
				image[i][j] = (byte)(0);
			}
		}
		for (int i = 0; i < 5; i++) {
			for (int j = 16; j < Height; j++) {
				image[j][i] = (byte)(2);
			}
		}
		//image[18][11] = 2;

		/*
		image[4][7] = 2;
		image[6][6] = 2;
		image[6][8] = 2;
		image[8][5] = 2;
		image[8][7] = 2;
		image[8][9] = 2;
		image[10][4] = 2;
		image[10][6] = 2;
		image[10][8] = 2;
		image[10][10] = 2;
		image[12][3] = 2;
		image[12][5] = 2;
		image[12][7] = 2;
		image[12][9] = 2;
		image[12][11] = 2;
		image[14][2] = 2;
		image[14][4] = 2;
		image[14][6] = 2;
		image[14][8] = 2;
		image[14][10] = 2;
		image[14][12] = 2;
		image[16][1] = 2;
		image[16][3] = 2;
		image[16][5] = 2;
		image[16][7] = 2;
		image[16][9] = 2;
		image[16][11] = 2;
		image[16][13] = 2;
		*/
		LCDScreen* scr = new LCDScreen(disp, image);
		do {
			//0<=y<=15 0<=x<=14
			//start point [0 row, 7 column]
			/*
			i
			0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
			0
			1
			2
			j    3
			4
			5
			..
			24
			*/
			for (int i = 0; i < Width; i++) {
				for (int j = Height - 1; j >= 0; j--) {
					if (image[j][i] == 1 && (j < Height - 1)) {
						if (image[j + 1][i] == 0) {
							image[j][i] = 0;
							image[j + 1][i] = 1;
						}
						else if (j < Height - 2) {
							int side = Drop(image, i, j);
							if (side != 0) {
								image[j][i] = 0;
								image[j + 1][i + side] = 1;
							}
						}
					}
					else {

					}
				}
			}

			//новая песчинка, если для нее есть место
			int pos = random() % Width;
			if (image[0][pos] == 0 && image[1][pos] == 0) {
				image[0][pos] = 1;
				skip = 0;
			}
			else {
				skip++;
				if (skip > 3) {
					scr->~LCDScreen();
					for (int i = 0; i < Height; i++) {
						delete[] image[i];
					}
					delete[] image;
					return;
				}
			}
			//обновление экрана
			scr->Draw(LCDScreen::Square_3x3_lb);

			for (int i = 0; i < Height; i++) {
				for (int j = 0; j < Width; j++) {
					if (image[i][j] == 1) cout << '0';
					else if (image[i][j] == 2) cout << '#';
					else cout << ' ';
				}
				cout << endl;
			}
			for (int i = 0; i < 1; i++) {
				for (int j = 0; j < Width; j++) {
					cout << '=';
				}
				cout << endl;
			}
			//cout << "----" << endl;
			this_thread::sleep_for(chrono::milliseconds(100));
		} while (1);
	}
}