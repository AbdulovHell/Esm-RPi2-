#include <wiringPi.h>
#include <lcd.h>

// ������� LED�� ������� 0 wiringPi ����� BCM_GPIO�17.
// ��� ������������� � �������������� wiringPiSetupSys ����� ��������� ��������� BCM
// ��� ������ ������� ���-���� ����������� ��������� BCM, �����
// �������� ������� "�������� �������"�� "������� ������"�� "��������� ������� ����� ������". 
// , ������� ���������� gpio export ��� ��������� �������� ��� wiringPiSetupSys
#define RW 27
#define RS 17
#define E 22

#define DB7 26
#define DB6 19
#define DB5 13
#define DB4 6

int main(void)
{
	wiringPiSetupSys();
	pinMode(RW, OUTPUT);
	digitalWrite(RW, LOW);
	int fd=lcdInit(16, 2, 4, RS, E, 0, 0, 0, 0, DB4, DB5, DB6, DB7);
	lcdPrintf(fd, "message");
	while (true)
	{
		;
	}
	return 0;
}