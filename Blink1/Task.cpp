#include "sys_headers.h"

#include "Task.h"
#include "Colorize.h"
#include "main.h"
#include "I2C.h"
#include "SPI.h"
#include "wiringPi.h"

using namespace std;

bool once = false;
IO::I2C* b3_8 = nullptr;
IO::I2C* b8_15 = nullptr;

void Threading::TaskSetAttCh::Run()
{
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%s: Set channel: %d Att: %d", Stuff::MakeColor("MAIN", Stuff::Green).c_str(), ch, att);
	cout << buf << endl;
}

void Threading::TaskQuit::Run()
{
	InitShutdown();
}

void Threading::TaskRequestTemp::Run()
{
	IO::I2C dev(0x10);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;
	else {
		short data = dev.Read2BytesFromReg(2);
		*TempStorage = data / 2.0;
	}
}

void Threading::TaskSetFreq::Run()
{
	if (b3_8 == nullptr) b3_8 = new IO::I2C(0x24);
	if (b8_15 == nullptr) b8_15 = new IO::I2C(0x25);

	//I2C
	uint8_t ch = 0;
	const uint8_t cmd = 1;
	if (Freq < 4500) ch = 1;
	else if (Freq < 6000) ch = 2;
	else if (Freq < 8000) ch = 3;
	else if (Freq < 12000) ch = 4;
	else if (Freq < 15000) ch = 5;
	else if (Freq < 18000) ch = 6;
	else if (Freq < 21000) ch = 7;
	else if (Freq < 24000) ch = 8;
	else if (Freq < 26500) ch = 9;
	else if (Freq < 30000) ch = 10;
	else if (Freq < 33000) ch = 11;
	else if (Freq < 36000) ch = 12;
	else if (Freq < 40000) ch = 13;

	b8_15->WriteReg(ch, 1);
	b8_15->WriteReg(cmd, 0);

	b3_8->WriteReg(ch, 1);
	b3_8->WriteReg(cmd, 0);
}

void Threading::TaskAdjustBL::Run()
{
	IO::I2C dev(0x10);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;
	else {
		uint8_t param = (127 + Step * 8);
		printf("New value: %d\n", param);
		dev.WriteReg(param, 1);
		dev.WriteReg((uint8_t)1, 0);
	}
}

void Threading::TaskSetPWM::Run()
{
	IO::I2C dev(0x10);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;
	else {
		uint8_t param = Step * 10;
		printf("Set pwm: %d%\n", param);
		dev.WriteReg(param, 4);
		dev.WriteReg((uint8_t)1, 0);
	}
}

void Threading::TaskSetOutput::Run()
{
	IO::I2C dev(0x24);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;
	else {
		dev.WriteReg(code, 3);
		dev.WriteReg((uint8_t)1, 0);
	}
}

void Threading::TaskSetAtt::Run()
{
	const uint8_t cmd = 1;
	//I2C
	if (b3_8 == nullptr) b3_8 = new IO::I2C(0x24);
	if (b8_15 == nullptr) b8_15 = new IO::I2C(0x25);
	//8-15
	b8_15->WriteReg(RFatt, 2);
	b8_15->WriteReg(cmd, 0);
	printf("Set att 8-15: %d\n", RFatt);
	//3-8
	if (RFatt > 15) RFatt = 15;
	uint8_t temp = (RFatt << 4) + (IFatt & 0xF);
	printf("Set att 3-8: 0x%X\n", temp);
	b3_8->WriteReg(temp, 2);
	b3_8->WriteReg(cmd, 0);	
}
