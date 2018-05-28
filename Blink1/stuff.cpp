#include "sys_headers.h"

#include "stuff.h"

namespace Stuff {
	Records* RecordsStorage;
}

bool Stuff::Verify(char * buf)
{
	if (buf[0] != 'e')  return false;	//базовая проверка
	size_t len = (buf[2] + (buf[3] << 8));	//получаем полезную длинну сообщения
	uint8_t mCK_A = buf[3 + len + 1], mCK_B = buf[3 + len + 2];	//суммы, поставляемые клиентом
	uint8_t cCK_A = 0, cCK_B = 0;	//наши суммы
	for (size_t i = 1; i < (3 + len + 1); i++) {//считаем
		cCK_A = (uint8_t)(cCK_A + (uint8_t)buf[i]);
		cCK_B = (uint8_t)(cCK_B + cCK_A);
	}
	if ((cCK_A == mCK_A) && (cCK_B == mCK_B)) //сравниваем
		return true;	//совпали
	return false;	//облом
}

void Stuff::CalcSum(char * buf, size_t size)
{
	uint8_t CK_A = 0, CK_B = 0;
	for (size_t i = 1; i < size - 2; i++) {
		CK_A = (uint8_t)(CK_A + (uint8_t)buf[i]);
		CK_B = (uint8_t)(CK_B + CK_A);
	}
	buf[size - 1] = CK_B;
	buf[size - 2] = CK_A;
}

int Stuff::RangeRand(int min, int max)
{
	max++;
	int rnd = rand();
	//printf("RR %d	",rnd);
	return (int)((double)rnd / ((double)RAND_MAX + 1.0)*(max - min) + min);
}

void Stuff::Records::UpdateFile()
{
	ofstream fs("rcrds", ios::out | ios::trunc | ios::binary);

	fs.write((char*)&SnakeRecord, 4);
	fs.write((char*)&SelfdestructRecord, 4);
	fs.write((char*)&RacingRecord, 4);
	fs.close();
}

Stuff::Records::Records()
{
	//попытка загрузки из файла
	ifstream ifs("rcrds", ios::in | ios::binary);
	//если файла нет, то установка по умолчанию
	if (!ifs) {
		Stuff::Records::SnakeRecord = 0;
		SelfdestructRecord = 0;
		RacingRecord = 0;
		TetrisRecord = 0;

		ofstream fs("rcrds", ios::out | ios::trunc | ios::binary);

		fs.write((char*)&SnakeRecord, 4);
		fs.write((char*)&SelfdestructRecord, 4);
		fs.write((char*)&RacingRecord, 4);
		fs.write((char*)&TetrisRecord, 4);
		fs.close();
		return;
	}

	//иначе парсим файлик
	char* input = new char[4 * 4];
	ifs.read(input, 4 * 4);

	memcpy(&SnakeRecord, input, 4);
	memcpy(&SelfdestructRecord, input + 4, 4);
	memcpy(&RacingRecord, input + 8, 4);
	memcpy(&TetrisRecord, input + 12, 4);

	ifs.close();
	delete[] input;
}

void Stuff::Records::SetSnakeRecord(int rec)
{
	SnakeRecord = rec;
	UpdateFile();
}

int Stuff::Records::GetSnakeRecord()
{
	return SnakeRecord;
}

void Stuff::Records::SetSelfdestructRecord(int rec)
{
	SelfdestructRecord = rec;
	UpdateFile();
}

int Stuff::Records::GetSelfdestructRecord()
{
	return SelfdestructRecord;
}

void Stuff::Records::SetRacingRecord(int rec)
{
	RacingRecord = rec;
	UpdateFile();
}

int Stuff::Records::GetRacingRecord()
{
	return RacingRecord;
}

void Stuff::Records::SetTetrisRecord(int rec)
{
	TetrisRecord = rec;
	UpdateFile();
}

int Stuff::Records::GetTetrisRecord()
{
	return TetrisRecord;
}
