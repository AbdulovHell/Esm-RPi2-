#include <wiringPi.h>
#include "lcd.h"

// ������� LED�� ������� 0 wiringPi ����� BCM_GPIO�17.
// ��� ������������� � �������������� wiringPiSetupSys ����� ��������� ��������� BCM
// ��� ������ ������� ���-���� ����������� ��������� BCM, �����
// �������� ������� "�������� �������"�� "������� ������"�� "��������� ������� ����� ������". 
// , ������� ���������� gpio export ��� ��������� �������� ��� wiringPiSetupSys
#define	LED	17

int main(void)
{
	wiringPiSetupSys();

	//pinMode(LED, OUTPUT);
	InitLCD();

	while (true)
	{
		ClearDisplay();
		//digitalWrite(LED, HIGH);  // ���.
		//delay(100); // ��
		//digitalWrite(LED, LOW);	  // ����.
		//delay(100);
	}
	return 0;
}