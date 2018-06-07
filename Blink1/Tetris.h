#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <stdlib.h>

#define STATIC 2
#define ACTIONFIGURE 1
#define EMPTY 0

namespace Games {

	class Figure {
	protected:
		std::pair<int, int> Center;

		bool Turn(unsigned char** image, int pos, int side) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 10; j++) {
					if (FigureSpace[i][j] == ACTIONFIGURE) {
						if (((j + side) < 0) || ((j + side) > 9)) {
							return false;
						}
					}
				}
			}

			unsigned char tempBuf[4][10];
			int jEnd = (side == -1) ? 9 : 10;
			int jSta = (side == -1) ? 0 : 1;
			int FillIndex = (side == -1) ? 9 : 0;
			for (int i = 0; i < 4; i++) {
				for (int j = jSta; j < jEnd; j++) {
					tempBuf[i][j] = FigureSpace[i][j - side];
				}
				tempBuf[i][FillIndex] = EMPTY;
			}

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 10; j++) {
					if (((i + pos) >= 0) && ((i + pos) < 32) && image[i + pos][j] == STATIC && tempBuf[i][j] == ACTIONFIGURE) {
						return false;
					}
				}
			}

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 10; j++) {
					FigureSpace[i][j] = tempBuf[i][j];
				}
			}

			return true;
		}

		unsigned char** RotateMatrix(unsigned char** input, int rows, int columns)
		{
			//int rows = input.Length;
			//int columns = input[0].Length;
			unsigned char** result = new unsigned char*[columns];
			for (int i = 0; i < columns; i++)
				result[i] = new unsigned char[rows];

			for (int i = 0, k = rows - 1; i < rows && k >= 0; i++, k--)
			{
				for (int j = 0; j < columns; j++)
				{
					result[j][k] = input[i][j];
				}
			}

			return result;
		}

		bool Collide(unsigned char** image, int pos, unsigned char** NewFigure) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 10; j++) {
					if (((i + pos) >= 0) && ((i + pos) < 32) && image[i + pos][j] == STATIC && NewFigure[i][j] == ACTIONFIGURE) {
						return true;
					}
				}
			}
			return false;
		}

		unsigned char** Copy() {
			unsigned char** temp = new unsigned char*[4];

			for (int i = 0; i < 4; i++) {
				temp[i] = new unsigned char[10];
				for (int j = 0; j < 10; j++) {
					temp[i][j] = FigureSpace[i][j];
				}
			}

			return temp;
		}

		void Delete(unsigned char** figure) {
			for (int i = 0; i < 4; i++)
				delete[] figure[i];
			delete[] figure;
		}

	public:
		unsigned char FigureSpace[4][10];

		Figure() {
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 10; j++) {
					FigureSpace[i][j] = EMPTY;
				}
		}

		virtual bool Rotate(unsigned char** image, int pos) = 0;
		virtual int InitPos() = 0;

		bool TurnLeft(unsigned char** image, int pos) {
			if (Turn(image, pos, -1)) {
				Center.second -= 1;
				return true;
			}
			return false;
		}

		bool TurnRight(unsigned char** image, int pos) {
			if (Turn(image, pos, 1)) {
				Center.second += 1;
				return true;
			}
			return false;
		}
	};

	class Cube : public Figure {

	public:
		Cube() : Figure() {
			FigureSpace[0][4] = ACTIONFIGURE; FigureSpace[0][5] = ACTIONFIGURE;
			FigureSpace[1][4] = ACTIONFIGURE; FigureSpace[1][5] = ACTIONFIGURE;
			Center.first = 0;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			return false;
		}

		int InitPos() {
			return -1;
		}
	};

	class Point : public Figure {

	public:
		Point() : Figure() {
			FigureSpace[0][4] = ACTIONFIGURE;
			Center.first = 0;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			return false;
		}

		int InitPos() {
			return 0;
		}
	};

	class TCross : public Figure {
	public:
		TCross() : Figure() {
			FigureSpace[0][4] = ACTIONFIGURE;
			FigureSpace[1][3] = ACTIONFIGURE; FigureSpace[1][4] = ACTIONFIGURE; FigureSpace[1][5] = ACTIONFIGURE;
			Center.first = 1;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			unsigned char** tempBuf = Copy();
			bool Rotated = false;

			if (tempBuf[Center.first - 1][Center.second] == 0) {
				tempBuf[Center.first - 1][Center.second] = 1;
				tempBuf[Center.first][Center.second + 1] = 0;
				Rotated = true;
			}
			else if ((Center.second + 1 < 10) && tempBuf[Center.first][Center.second + 1] == 0) {
				tempBuf[Center.first][Center.second + 1] = 1;
				tempBuf[Center.first + 1][Center.second] = 0;
				Rotated = true;
			}
			else if (tempBuf[Center.first + 1][Center.second] == 0) {
				tempBuf[Center.first + 1][Center.second] = 1;
				tempBuf[Center.first][Center.second - 1] = 0;
				Rotated = true;
			}
			else if ((Center.second - 1 >= 0) && tempBuf[Center.first][Center.second - 1] == 0) {
				tempBuf[Center.first][Center.second - 1] = 1;
				tempBuf[Center.first - 1][Center.second] = 0;
				Rotated = true;
			}

			if (Rotated && !Collide(image, pos, tempBuf)) {
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 10; j++)
						FigureSpace[i][j] = tempBuf[i][j];
				Delete(tempBuf);
				return true;
			}
			Delete(tempBuf);
			return false;
		}

		int InitPos() {
			return -2;
		}
	};

	class Line : public Figure {
	public:
		Line() : Figure() {
			FigureSpace[0][4] = ACTIONFIGURE;
			FigureSpace[1][4] = ACTIONFIGURE;
			FigureSpace[2][4] = ACTIONFIGURE;
			FigureSpace[3][4] = ACTIONFIGURE;
			Center.first = 2;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			unsigned char** tempBuf = Copy();
			bool Rotated = false;

			if ((Center.second - 2 >= 0) && (Center.second + 1 < 10) && tempBuf[Center.first - 1][Center.second] == 1) {	//вертикаль
				tempBuf[Center.first - 1][Center.second] = 0;
				tempBuf[Center.first - 2][Center.second] = 0;
				tempBuf[Center.first + 1][Center.second] = 0;
				tempBuf[Center.first][Center.second - 1] = 1;
				tempBuf[Center.first][Center.second - 2] = 1;
				tempBuf[Center.first][Center.second + 1] = 1;
				Rotated = true;
			}
			else if ((Center.second + 1 < 10) && tempBuf[Center.first][Center.second + 1] == 1) {
				tempBuf[Center.first - 1][Center.second] = 1;
				tempBuf[Center.first - 2][Center.second] = 1;
				tempBuf[Center.first + 1][Center.second] = 1;
				tempBuf[Center.first][Center.second - 1] = 0;
				tempBuf[Center.first][Center.second - 2] = 0;
				tempBuf[Center.first][Center.second + 1] = 0;
				Rotated = true;
			}

			if (Rotated && !Collide(image, pos, tempBuf)) {
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 10; j++)
						FigureSpace[i][j] = tempBuf[i][j];
				Delete(tempBuf);
				return true;
			}
			Delete(tempBuf);
			return false;
		}

		int InitPos() {
			return -3;
		}
	};

	class LeftTurn : public Figure {
	public:
		LeftTurn() : Figure() {
			FigureSpace[0][3] = ACTIONFIGURE; FigureSpace[0][4] = ACTIONFIGURE;
			FigureSpace[1][4] = ACTIONFIGURE;
			FigureSpace[2][4] = ACTIONFIGURE;
			Center.first = 1;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			unsigned char** tempBuf = Copy();
			bool Rotated = false;

			if (Center.second != 0 && Center.second != 9) {
				if (Center.second - 1 >= 0 && tempBuf[Center.first - 1][Center.second - 1] == 1) {
					tempBuf[Center.first - 1][Center.second - 1] = 0;
					tempBuf[Center.first - 1][Center.second] = 0;
					tempBuf[Center.first + 1][Center.second] = 0;
					tempBuf[Center.first - 1][Center.second + 1] = 1;
					tempBuf[Center.first][Center.second - 1] = 1;
					tempBuf[Center.first][Center.second + 1] = 1;
					Rotated = true;
				}
				else if ((Center.second + 1 < 10) && tempBuf[Center.first - 1][Center.second + 1] == 1) {
					tempBuf[Center.first - 1][Center.second + 1] = 0;
					tempBuf[Center.first][Center.second - 1] = 0;
					tempBuf[Center.first][Center.second + 1] = 0;
					tempBuf[Center.first + 1][Center.second] = 1;
					tempBuf[Center.first + 1][Center.second + 1] = 1;
					tempBuf[Center.first - 1][Center.second] = 1;
					Rotated = true;
				}
				else if ((Center.second + 1 < 10) && tempBuf[Center.first + 1][Center.second + 1] == 1) {
					tempBuf[Center.first + 1][Center.second + 1] = 0;
					tempBuf[Center.first + 1][Center.second] = 0;
					tempBuf[Center.first - 1][Center.second] = 0;
					tempBuf[Center.first + 1][Center.second - 1] = 1;
					tempBuf[Center.first][Center.second - 1] = 1;
					tempBuf[Center.first][Center.second + 1] = 1;
					Rotated = true;
				}
				else if (Center.second - 1 >= 0 && tempBuf[Center.first + 1][Center.second - 1] == 1) {
					tempBuf[Center.first + 1][Center.second - 1] = 0;
					tempBuf[Center.first][Center.second - 1] = 0;
					tempBuf[Center.first][Center.second + 1] = 0;
					tempBuf[Center.first - 1][Center.second - 1] = 1;
					tempBuf[Center.first - 1][Center.second] = 1;
					tempBuf[Center.first + 1][Center.second] = 1;
					Rotated = true;
				}
			}
			if (Rotated && !Collide(image, pos, tempBuf)) {
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 10; j++)
						FigureSpace[i][j] = tempBuf[i][j];
				Delete(tempBuf);
				return true;
			}
			Delete(tempBuf);
			return false;
		}

		int InitPos() {
			return -2;
		}
	};

	class RightTurn : public Figure {
	public:
		RightTurn() : Figure() {
			FigureSpace[0][5] = ACTIONFIGURE; FigureSpace[0][4] = ACTIONFIGURE;
			FigureSpace[1][4] = ACTIONFIGURE;
			FigureSpace[2][4] = ACTIONFIGURE;
			Center.first = 1;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			unsigned char** tempBuf = Copy();
			bool Rotated = false;

			if (Center.second != 0 && Center.second != 9) {
				if (Center.second + 1 < 10 && tempBuf[Center.first - 1][Center.second + 1] == 1) {
					tempBuf[Center.first - 1][Center.second + 1] = 0;
					tempBuf[Center.first - 1][Center.second] = 0;
					tempBuf[Center.first + 1][Center.second] = 0;
					tempBuf[Center.first + 1][Center.second + 1] = 1;
					tempBuf[Center.first][Center.second - 1] = 1;
					tempBuf[Center.first][Center.second + 1] = 1;
					Rotated = true;
				}
				else if ((Center.second + 1 < 10) && tempBuf[Center.first + 1][Center.second + 1] == 1) {
					tempBuf[Center.first + 1][Center.second + 1] = 0;
					tempBuf[Center.first][Center.second - 1] = 0;
					tempBuf[Center.first][Center.second + 1] = 0;
					tempBuf[Center.first + 1][Center.second] = 1;
					tempBuf[Center.first + 1][Center.second - 1] = 1;
					tempBuf[Center.first - 1][Center.second] = 1;
					Rotated = true;
				}
				else if ((Center.second - 1 >= 0) && tempBuf[Center.first + 1][Center.second - 1] == 1) {
					tempBuf[Center.first + 1][Center.second - 1] = 0;
					tempBuf[Center.first + 1][Center.second] = 0;
					tempBuf[Center.first - 1][Center.second] = 0;
					tempBuf[Center.first - 1][Center.second - 1] = 1;
					tempBuf[Center.first][Center.second - 1] = 1;
					tempBuf[Center.first][Center.second + 1] = 1;
					Rotated = true;
				}
				else if (Center.second - 1 >= 0 && tempBuf[Center.first - 1][Center.second - 1] == 1) {
					tempBuf[Center.first - 1][Center.second - 1] = 0;
					tempBuf[Center.first][Center.second - 1] = 0;
					tempBuf[Center.first][Center.second + 1] = 0;
					tempBuf[Center.first - 1][Center.second + 1] = 1;
					tempBuf[Center.first - 1][Center.second] = 1;
					tempBuf[Center.first + 1][Center.second] = 1;
					Rotated = true;
				}
			}
			if (Rotated && !Collide(image, pos, tempBuf)) {
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 10; j++)
						FigureSpace[i][j] = tempBuf[i][j];
				Delete(tempBuf);
				return true;
			}
			Delete(tempBuf);
			return false;
		}

		int InitPos() {
			return -2;
		}
	};

	class Z1 : public Figure {
	public:
		Z1() : Figure() {
			FigureSpace[0][3] = ACTIONFIGURE; FigureSpace[0][4] = ACTIONFIGURE;
			FigureSpace[1][4] = ACTIONFIGURE;	FigureSpace[1][5] = ACTIONFIGURE;
			Center.first = 1;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			unsigned char** tempBuf = Copy();
			bool Rotated = false;

			if (Center.second != 0 && Center.second != 9) {
				unsigned char** f = new unsigned char*[3];
				f[0] = new unsigned char[3];
				f[1] = new unsigned char[3];
				f[2] = new unsigned char[3];

				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						f[i][j] = tempBuf[i + Center.first - 1][j + Center.second - 1];
					}
				}

				unsigned char** newF = RotateMatrix(f, 3, 3);

				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						tempBuf[i + Center.first - 1][j + Center.second - 1] = newF[i][j];
					}
				}

				Rotated = true;
			}

			if (Rotated && !Collide(image, pos, tempBuf)) {
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 10; j++)
						FigureSpace[i][j] = tempBuf[i][j];
				Delete(tempBuf);
				return true;
			}
			Delete(tempBuf);
			return false;
		}

		int InitPos() {
			return -1;
		}
	};

	class Z2 : public Figure {
	public:
		Z2() : Figure() {
			FigureSpace[0][4] = ACTIONFIGURE; FigureSpace[0][5] = ACTIONFIGURE;
			FigureSpace[1][3] = ACTIONFIGURE;	FigureSpace[1][4] = ACTIONFIGURE;
			Center.first = 1;
			Center.second = 4;
		}

		bool Rotate(unsigned char** image, int pos) {
			unsigned char** tempBuf = Copy();
			bool Rotated = false;

			if (Center.second != 0 && Center.second != 9) {
				unsigned char** f = new unsigned char*[3];
				f[0] = new unsigned char[3];
				f[1] = new unsigned char[3];
				f[2] = new unsigned char[3];

				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						f[i][j] = tempBuf[i + Center.first - 1][j + Center.second - 1];
					}
				}

				unsigned char** newF = RotateMatrix(f, 3, 3);

				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						tempBuf[i + Center.first - 1][j + Center.second - 1] = newF[i][j];
					}
				}

				Rotated = true;
			}

			if (Rotated && !Collide(image, pos, tempBuf)) {
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 10; j++)
						FigureSpace[i][j] = tempBuf[i][j];
				Delete(tempBuf);
				return true;
			}
			Delete(tempBuf);
			return false;
		}

		int InitPos() {
			return -1;
		}
	};

	Figure* CreateRandom() {
		switch (random() % 8)
		{
		case 0:
			return new Line();
		case 1:
			return new Cube();
		case 2:
			return new TCross();
		case 3:
			return new LeftTurn();
		case 4:
			return new RightTurn();
		case 5:
			return new Point();
		case 6:
			return new Z1();
		case 7:
			return new Z2();
		default:
			return new Cube();
		}
	}
}

#endif
