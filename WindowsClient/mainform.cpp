#include <stdint.h>
#include "mainform.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Net;
using namespace System::Net::Sockets;

[STAThreadAttribute]
void Main(cli::array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	WindowsClient::mainform form;
	Application::Run(%form);
}

void WindowsClient::mainform::WriteLog(String ^ msg)
{
	ProgLog->AppendText(msg + "\n");
}

void WindowsClient::mainform::ReadSocket()
{
	while (isConnected) {
		array<Byte>^ data = gcnew array<Byte>(256);
		String^ response = String::Empty;
		int bytes = stream->Read(data, 0, data->Length);
		response = System::Text::Encoding::ASCII->GetString(data, 0, bytes);

		if (!Verify(data))
			this->Invoke(gcnew Action<String^>(this, &mainform::WriteLog), response);
		else
			switch (data[2]) {
			case 0:
			{
				switch (data[3]) {
				case 0:
				{
					initTest = false;
				}
				break;
				default:
					break;
				}
			}
			break;
			default:
				break;
			}
	}
}

void WindowsClient::mainform::SendCMD(uint8_t ch, uint8_t att)
{
	array<Byte>^ data = gcnew array<Byte>(10);	//2+1+1+2+1+1+2  header+mc+mi+len+ch+att+sum
	//header
	data[0] = 'e';
	data[1] = 's';
	//message class
	data[2] = 1;
	//message id
	data[3] = 0;
	//len = 2
	data[4] = 2;
	data[5] = 0;
	//ch & att
	data[6] = ch;
	data[7] = att;
	CalcSum(data);
	stream->Write(data, 0, data->Length);
}

void WindowsClient::mainform::Test()
{
	array<Byte>^ data = gcnew array<Byte>(8);
	data[0] = 'e';
	data[1] = 's';
	//message class
	data[2] = 0;
	//message id
	data[3] = 0;
	//len = 0
	data[4] = 0;
	data[5] = 0;
	CalcSum(data);
	stream->Write(data, 0, data->Length);
}

void WindowsClient::mainform::CalcSum(array<Byte>^ dt)
{
	uint8_t CK_A = 0, CK_B = 0;
	for (size_t i = 2; i < dt->Length - 2; i++) {
		CK_A += dt[i];
		CK_B += CK_A;
	}
	dt[dt->Length - 1] = CK_B;
	dt[dt->Length - 2] = CK_A;
}

bool WindowsClient::mainform::Verify(array<Byte>^ dt)
{
	if (dt[0] != 'e' || dt[1] != 's')
		return false;

	uint8_t mCK_A = dt[dt->Length - 2], mCK_B = dt[dt->Length - 1];
	uint8_t CK_A = 0, CK_B = 0;
	for (size_t i = 2; i < dt->Length - 2; i++) {
		CK_A += dt[i];
		CK_B += CK_A;
	}

	if (mCK_A != CK_A || mCK_B != CK_B)
		return false;

	return true;
}

System::Void WindowsClient::mainform::button1_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	int port = 4550;	//default
	if (!int::TryParse(PortEdit->Text, port))
		return;
	client = gcnew TcpClient(IPAddrEdit->Text, port);
	stream = client->GetStream();
	isConnected = true;
	ConnTest->Enabled = true;
	readThrd = gcnew Thread(gcnew ThreadStart(this, &mainform::ReadSocket));
	readThrd->Start();
}

System::Void WindowsClient::mainform::button2_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	array<Byte>^ data = System::Text::Encoding::ASCII->GetBytes(textBox3->Text);
	stream->Write(data, 0, data->Length);
}

System::Void WindowsClient::mainform::mainform_FormClosed(System::Object ^ sender, System::Windows::Forms::FormClosedEventArgs ^ e)
{
	try {
		client->Close();
	}
	catch (...) {

	}
}

System::Void WindowsClient::mainform::SendParamBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	uint8_t att, ch;
	if (ChannelNum->SelectedIndex < 0 || ChannelNum->SelectedIndex>40) {
		WriteLog("Неправильный номер канала");
		return;
	}
	else {
		ch = ChannelNum->SelectedIndex;
	}
	att = Decimal::ToByte(AttNum->Value);
	SendCMD(ch, att);
	WriteLog("Ch:" + ch + " At:" + att);
}

System::Void WindowsClient::mainform::ConnTest_Tick(System::Object ^ sender, System::EventArgs ^ e)
{
	if (initTest) {
		isConnected = false;
		ConnTest->Enabled = false;
	}
	else {
		Test();
		initTest = true;
	}
}

