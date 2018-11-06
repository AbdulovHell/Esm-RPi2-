#include "sys_headers.h"

#include "Task.h"
#include "Colorize.h"
#include "main.h"
#include "I2C.h"
#include "usart.h"
#include "wiringPi.h"

using namespace std;

IO::I2C* b3_8 = nullptr;
IO::I2C* b8_15 = nullptr;
IO::I2C* b15_30 = nullptr;
IO::I2C* b_bfg = nullptr;
IO::I2C* b_bfg23 = nullptr;
IO::Usart* Elite = nullptr;

void Threading::TaskSetAttCh::Run()
{
	//not used
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
	//I2C
	if (b3_8 == nullptr) b3_8 = new IO::I2C(0x24);
	if (b8_15 == nullptr) b8_15 = new IO::I2C(0x25);
	if (b15_30 == nullptr) b15_30 = new IO::I2C(0x28);
	if (b_bfg == nullptr) b_bfg = new IO::I2C(0x26);
	if (b_bfg23 == nullptr) b_bfg23 = new IO::I2C(0x27);

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

	b15_30->WriteReg(ch, 1);
	b15_30->WriteReg(cmd, 0);

	b8_15->WriteReg(ch, 1);
	b8_15->WriteReg(cmd, 0);

	b3_8->WriteReg(ch, 1);
	b3_8->WriteReg(cmd, 0);

	int freq = Freq;
	printf("%s: BFG freq %d\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str(), freq);
	uint8_t num[4] = { 0, };
	memcpy(num, &freq, 4);
	printf("%s: [%2X] [%2X] [%2X] [%2X]\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str(), num[0], num[1], num[2], num[3]);
	//bfg
	for (int i = 0; i < 4; i++) b_bfg->WriteReg(num[i], 4 + i);
	b_bfg->WriteReg(cmd, 0);
	//bfg23
	for (int i = 0; i < 4; i++) b_bfg23->WriteReg(num[i], 4 + i);
	b_bfg23->WriteReg(cmd, 0);

	printf("%s: I2C OK freq\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
	//USART
	if (Elite == nullptr) Elite = new IO::Usart(9600);
	if (Elite->IsOpen()) {

		char buf[32] = { 0, };
		buf[0] = 'e';
		buf[1] = 2;	//freq
		memcpy(buf + 8, &Freq, sizeof(Freq));
		Elite->Write(buf, 32);

		//wait answer 500 ms async
		//or 50 ms sleep 

		this_thread::sleep_for(std::chrono::milliseconds(500));
		int size = Elite->DataAvail();
		if (size > 1) {
			char* rBuf = new char[size];
			Elite->Read(rBuf, size);

			if (rBuf[0] == 2 && rBuf[1] == 1) {
				printf("%s: Elite OK freq\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
			}
			else if (rBuf[0] == 2 && rBuf[1] == 2) {
				printf("%s: Elite bad freq\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
			}
			else {
				printf("%s: Elite TXRX corrupted\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
			}

			delete[] rBuf;
		}
		else {
			printf("%s: Elite not respond\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
		}
	}
	else {
		printf("%s: Elite not connected! %s\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
		cout << Elite->GetError() << endl;
	}

	printf("%s: Set channel: %d ; Freq: %d MHz\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str(), ch, Freq);
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
	if (b3_8 == nullptr) b3_8 = new IO::I2C(0x24);
	if (b_bfg23 == nullptr) b_bfg23 = new IO::I2C(0x27);

	b3_8->WriteReg(code, 3);
	b3_8->WriteReg((uint8_t)1, 0);

	b_bfg23->WriteReg(code, 3);
	b_bfg23->WriteReg((uint8_t)1, 0);
}

void Threading::TaskSetAtt::Run()
{
	const uint8_t cmd = 1;
	//I2C
	if (b3_8 == nullptr) b3_8 = new IO::I2C(0x24);
	if (b8_15 == nullptr) b8_15 = new IO::I2C(0x25);
	if (b15_30 == nullptr) b15_30 = new IO::I2C(0x28);
	//if (b_bfg == nullptr) b_bfg = new IO::I2C(0x26);
	//15-30
	b15_30->WriteReg(RFatt, 2);
	b15_30->WriteReg(cmd, 0);
	printf("Set att 15-30: %d\n", RFatt);
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

void Threading::TaskChangeRef::Run()
{
}
