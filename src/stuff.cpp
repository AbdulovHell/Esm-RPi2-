#include <stdint.h>
#include <stddef.h>
#include "stuff.h"

bool Stuff::Verify(char * buf)
{
	if (buf[0] != 'e')  return false;	//������� ��������
	size_t len = (buf[2] + (buf[3] << 8));	//�������� �������� ������ ���������
	uint8_t mCK_A = buf[3 + len + 1], mCK_B = buf[3 + len + 2];	//�����, ������������ ��������
	uint8_t cCK_A = 0, cCK_B = 0;	//���� �����
	for (size_t i = 1; i < (3 + len + 1); i++) {//�������
		cCK_A = (uint8_t)(cCK_A + (uint8_t)buf[i]);
		cCK_B = (uint8_t)(cCK_B + cCK_A);
	}
	if ((cCK_A == mCK_A) && (cCK_B == mCK_B)) //����������
		return true;	//�������
	return false;	//�����
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

