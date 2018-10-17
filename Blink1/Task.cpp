#include "sys_headers.h"

#include "Task.h"
#include "Colorize.h"
#include "main.h"
#include "I2C.h"
#include "SPI.h"
#include "wiringPi.h"

using namespace std;

bool once = false;
IO::SPI* dev;
IO::I2C* b3_8 = nullptr;

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
	if (!once) {
		once = true;
		//init spi
		dev = new IO::SPI(0, 19200);
		pinMode(25, OUTPUT);
		digitalWrite(25, 1);
		this_thread::sleep_for(std::chrono::milliseconds(100));

		cout << "send ref out on" << endl;
		{
			this_thread::sleep_for(std::chrono::milliseconds(1));
			//ref out on
			//set chip select
			digitalWrite(25, 0);
			this_thread::sleep_for(std::chrono::microseconds(10));
			//send data
			uint8_t buf1[2] = { 0x08,1 };
			dev->DataRW(buf1, 2);
			//reset chip select
			this_thread::sleep_for(std::chrono::microseconds(10));
			digitalWrite(25, 1);
		}
		{
			this_thread::sleep_for(std::chrono::milliseconds(1));
			//ref out on
			//set chip select
			digitalWrite(25, 0);
			this_thread::sleep_for(std::chrono::microseconds(10));
			//send data
			uint8_t buf1[2] = { 0x08,1 };
			dev->DataRW(buf1, 2);
			//reset chip select
			this_thread::sleep_for(std::chrono::microseconds(10));
			digitalWrite(25, 1);
		}
		cout << "send RF out on" << endl;
		{
			this_thread::sleep_for(std::chrono::milliseconds(1));
			//RF out on
			//set chip select
			digitalWrite(25, 0);
			this_thread::sleep_for(std::chrono::microseconds(10));
			//send data
			uint8_t buf2[2] = { 0x0F,1 };
			dev->DataRW(buf2, 2);
			//reset chip select
			this_thread::sleep_for(std::chrono::microseconds(10));
			digitalWrite(25, 1);
		}
		{
			this_thread::sleep_for(std::chrono::milliseconds(1));
			//RF out on
			//set chip select
			digitalWrite(25, 0);
			this_thread::sleep_for(std::chrono::microseconds(10));
			//send data
			uint8_t buf2[2] = { 0x0F,1 };
			dev->DataRW(buf2, 2);
			//reset chip select
			this_thread::sleep_for(std::chrono::microseconds(10));
			digitalWrite(25, 1);
		}
		//I2C
		if (b3_8 == nullptr) b3_8 = new IO::I2C(0x24);
	}

	cout << "send freq" << endl;
	{
		this_thread::sleep_for(std::chrono::milliseconds(1));
		//set chip select
		digitalWrite(25, 0);
		this_thread::sleep_for(std::chrono::microseconds(10));
		//send data
		double Units = Freq * 1000000000.0;
		printf("Units : %f \n", Units);
		uint8_t ubuf[8];
		uint8_t buf[7] = { 0x0C,0,0,0,0,0,0 };
		uint64_t iUnits = Units;
		printf("iUnits : %u64 \n", iUnits);
		memcpy(ubuf, &iUnits, 8);
		for (int i = 1, j = 5; i < 7; i++, j--)
			buf[i] = ubuf[j];
		printf("%X: %X %X %X %X %X %X \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
		dev->DataRW(buf, 7);
		//reset chip select
		this_thread::sleep_for(std::chrono::microseconds(10));
		digitalWrite(25, 1);
	}
	{
		this_thread::sleep_for(std::chrono::microseconds(10));
		//set chip select
		digitalWrite(25, 0);
		this_thread::sleep_for(std::chrono::microseconds(10));
		//send data
		uint64_t Units = (uint64_t)Freq*(uint64_t)1000000000;
		uint8_t ubuf[8];
		memcpy(ubuf, &Units, 8);
		uint8_t buf[7] = { 0x0C,0,0,0,0,0,0 };
		for (int i = 1, j = 7; i < 7; i++, j--)
			buf[i] = ubuf[j];
		dev->DataRW(buf, 7);
		//reset chip select
		this_thread::sleep_for(std::chrono::microseconds(10));
		digitalWrite(25, 1);
	}
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
	//I2C
	if (b3_8 == nullptr) b3_8 = new IO::I2C(0x24);
	//I2C
	printf("Set att: 0x%X\n", att);
	const uint8_t cmd = 1;
	b3_8->WriteReg(att, 2);
	b3_8->WriteReg(cmd, 0);
}
