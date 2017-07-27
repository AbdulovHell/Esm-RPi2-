#pragma once

namespace WindowsClient {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Net;
	using namespace System::Net::Sockets;
	using namespace System::Threading;
	/// <summary>
	/// —водка дл€ mainform
	/// </summary>
	public ref class mainform : public System::Windows::Forms::Form
	{
	public:
		mainform(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~mainform()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBox1;
	protected:
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::TextBox^  textBox4;

	private:
		void WriteLog(String^ msg);
		void ReadSocket();
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>
		TcpClient^ client;
		NetworkStream^ stream;
		Thread^ readThrd;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::ComponentModel::IContainer^  components;


#pragma region Windows Form Designer generated code
			 /// <summary>
			 /// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
			 /// содержимое этого метода с помощью редактора кода.
			 /// </summary>
			 void InitializeComponent(void)
			 {
				 this->components = (gcnew System::ComponentModel::Container());
				 this->textBox1 = (gcnew System::Windows::Forms::TextBox());
				 this->textBox2 = (gcnew System::Windows::Forms::TextBox());
				 this->button1 = (gcnew System::Windows::Forms::Button());
				 this->textBox3 = (gcnew System::Windows::Forms::TextBox());
				 this->button2 = (gcnew System::Windows::Forms::Button());
				 this->textBox4 = (gcnew System::Windows::Forms::TextBox());
				 this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
				 this->SuspendLayout();
				 // 
				 // textBox1
				 // 
				 this->textBox1->Location = System::Drawing::Point(12, 12);
				 this->textBox1->Name = L"textBox1";
				 this->textBox1->Size = System::Drawing::Size(105, 20);
				 this->textBox1->TabIndex = 0;
				 this->textBox1->Text = L"192.168.1.44";
				 // 
				 // textBox2
				 // 
				 this->textBox2->Location = System::Drawing::Point(123, 12);
				 this->textBox2->Name = L"textBox2";
				 this->textBox2->Size = System::Drawing::Size(73, 20);
				 this->textBox2->TabIndex = 1;
				 this->textBox2->Text = L"4550";
				 // 
				 // button1
				 // 
				 this->button1->Location = System::Drawing::Point(202, 12);
				 this->button1->Name = L"button1";
				 this->button1->Size = System::Drawing::Size(75, 23);
				 this->button1->TabIndex = 2;
				 this->button1->Text = L"button1";
				 this->button1->UseVisualStyleBackColor = true;
				 this->button1->Click += gcnew System::EventHandler(this, &mainform::button1_Click);
				 // 
				 // textBox3
				 // 
				 this->textBox3->Location = System::Drawing::Point(12, 38);
				 this->textBox3->Multiline = true;
				 this->textBox3->Name = L"textBox3";
				 this->textBox3->Size = System::Drawing::Size(184, 78);
				 this->textBox3->TabIndex = 3;
				 // 
				 // button2
				 // 
				 this->button2->Location = System::Drawing::Point(202, 93);
				 this->button2->Name = L"button2";
				 this->button2->Size = System::Drawing::Size(75, 23);
				 this->button2->TabIndex = 4;
				 this->button2->Text = L"button2";
				 this->button2->UseVisualStyleBackColor = true;
				 this->button2->Click += gcnew System::EventHandler(this, &mainform::button2_Click);
				 // 
				 // textBox4
				 // 
				 this->textBox4->Location = System::Drawing::Point(12, 139);
				 this->textBox4->Multiline = true;
				 this->textBox4->Name = L"textBox4";
				 this->textBox4->Size = System::Drawing::Size(446, 165);
				 this->textBox4->TabIndex = 5;
				 // 
				 // timer1
				 // 
				 this->timer1->Interval = 10;
				 this->timer1->Tick += gcnew System::EventHandler(this, &mainform::timer1_Tick);
				 // 
				 // mainform
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(470, 316);
				 this->Controls->Add(this->textBox4);
				 this->Controls->Add(this->button2);
				 this->Controls->Add(this->textBox3);
				 this->Controls->Add(this->button1);
				 this->Controls->Add(this->textBox2);
				 this->Controls->Add(this->textBox1);
				 this->Name = L"mainform";
				 this->Text = L"mainform";
				 this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &mainform::mainform_FormClosed);
				 this->ResumeLayout(false);
				 this->PerformLayout();

			 }
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void mainform_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e);
	};
}
