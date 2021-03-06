#include "sys_headers.h"

#include "Task.h"
#include "Colorize.h"
#include "main.h"
#include "I2C.h"
#include "usart.h"
#include "wiringPi.h"
#include "settings.h"

using namespace std;

IO::I2C* b3_8 = nullptr;
IO::I2C* b8_15 = nullptr;
IO::I2C* b15_30 = nullptr;
IO::I2C* b_bfg = nullptr;
IO::I2C* b_bfg23 = nullptr;
IO::Usart* Elite = nullptr;

float ToFsint(float freq) {
	if (freq >= 3000.0 && freq < 3515.0) {
		return (freq + 1485.0)*2.0;
	}
	else if (freq>=3515.0 && freq<8000.0) {
		return freq + 1485.0;
	}
	else if (freq >= 8000.0 && freq < 15000.0) {
		float temp = (freq + 5015.0)/2.0;
		return temp;
	}
	else if (freq >= 15000.0 && freq < 19000.0) {
		float temp = (freq + 5015.0)/4.0;
		return temp;
	}
	else if (freq >= 19000.0 && freq < 25015.0) {
		float temp = (freq - 5015.0) / 2.0;
		return temp;
	}
	else if (freq >= 25015.0 && freq <= 30000.0) {
		float temp = (freq - 5015.0) / 4.0;
		return temp;
	}
	else
		return 3333.0;
}

int SetEliteFreq(float Freq) {
	int attempts = 0;
	//USART
	if (Elite == nullptr) Elite = new IO::Usart(9600);
	if (Elite->IsOpen()) {		
		char buf[32] = { 0, };
		buf[0] = 'e';
		buf[1] = 2;	//freq

		if (3000.0 <= Freq && Freq < 8000.0)
			buf[2] = 3;	//Output 3
		else if (8000.0 <= Freq && Freq < 30000.0)
			buf[2] = 1;	//Output 1
		else
			buf[2] = 2;	//Output 2

		float Fsint = ToFsint(Freq);
		memcpy(buf + 8, &Fsint, sizeof(Fsint));

		do {
			Elite->Write(buf, 32);

			//wait answer 500 ms async
			//or 500 ms sleep 

			this_thread::sleep_for(std::chrono::milliseconds(500));
			int size = Elite->DataAvail();
			if (size > 1) {
				char* rBuf = new char[size];
				Elite->Read(rBuf, size);

				if (rBuf[0] == 2 && rBuf[1] == 1) {
					printf("%s: Elite OK freq\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
					return attempts;
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
		}while(++attempts<3);
	}
	else {
		printf("%s: Elite not connected! %s\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
		cout << Elite->GetError() << endl;
	}
	return attempts;
}

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
		//cout << "Raw temp num: " << data << endl;
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

	/*float freqs[] = {3000,6000,8000,9000,10000,12000,15000,16000,18000,19000,20000,22000,24000,25000,26000,27000,29000,30000};
	float ans[18] = { 0, };
	for (int i = 0; i < 18; i++) 
		ans[i] = ToFsint(freqs[i]);*/

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
	
	SetEliteFreq(Freq);

	printf("%s: Set channel: %d ; Freq: %d MHz\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str(), ch, Freq);
	Stuff::Storage->SetFreq(Freq);
}

void Threading::TaskAdjustBL::Run()
{
	IO::I2C dev(0x10);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;
	else {
		uint8_t param = (127 + Step * 8);
		printf("New BL value: %d\n", param);
		dev.WriteReg(param, 1);
		dev.WriteReg((uint8_t)1, 0);
	}
	Stuff::Storage->SetBrightLvl(Step);
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

	Stuff::Storage->SetIF(code);
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

	Stuff::Storage->SetAtt(RFatt, IFatt);
}

void Threading::TaskChangeRef::Run()
{
	int attempts = 0;
	//USART
	if (Elite == nullptr) Elite = new IO::Usart(9600);
	if (Elite->IsOpen()) {
		char buf[32] = { 0, };
		buf[0] = 'e';
		buf[1] = 3;	//Ref
		buf[2] = code;

		do {
			Elite->Write(buf, 32);

			//wait answer 500 ms async
			//or 500 ms sleep 

			this_thread::sleep_for(std::chrono::milliseconds(200));
			int size = Elite->DataAvail();
			if (size > 1) {
				char* rBuf = new char[size];
				Elite->Read(rBuf, size);

				if (rBuf[0] == 3 && rBuf[1] == 1) {
					printf("%s: Elite OK change ref to %s\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str(),(code)?"Ext":"Int");
				}
				else {
					printf("%s: Elite TXRX corrupted\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
				}

				delete[] rBuf;
			}
			else {
				printf("%s: Elite not respond\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
			}
		} while (++attempts<3);
	}
	else {
		printf("%s: Elite not connected! %s\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
		cout << Elite->GetError() << endl;
	}

	Stuff::Storage->SetRef(code);
}
