#ifndef _STUFF_H_
#define _STUFF_H_

namespace Stuff {
	
	using namespace std;

	class Records;

	extern Records* RecordsStorage;

	//��������� �������� ���
	class Records {
		int SnakeRecord;
		int SelfdestructRecord;
		int RacingRecord;
		int TetrisRecord;
		
		void UpdateFile();

	public:

		Records();

		void SetSnakeRecord(int rec);
		int GetSnakeRecord();

		void SetSelfdestructRecord(int rec);
		int GetSelfdestructRecord();

		void SetRacingRecord(int rec);
		int GetRacingRecord();

		void SetTetrisRecord(int rec);
		int GetTetrisRecord();
	};

	//�������� ����������� �����
	bool Verify(char* buf);
	//������� ����������� �����
	void CalcSum(char* buf, size_t size);
	//��������� ��������� �����
	int RangeRand(int min, int max);
}
#endif	// _STUFF_H_
