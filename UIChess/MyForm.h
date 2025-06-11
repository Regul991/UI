#pragma once

namespace UIChess {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			CreateButtons();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

	/*-------------------------------------------
			Создаем массив из 64 кнопок
	-------------------------------------------*/
	private: 
		cli::array<Button^, 2>^ buttons;
		void CreateButtons()
		{
			buttons = gcnew cli::array<Button^, 2>(8, 8);

			for (int row = 0; row < 8; row++) // 8 строк
			{
				for (int col = 0; col < 8; col++) // 8 столбцов 
				{
					Button^ btn = gcnew Button(); // Функция добавления нового управляемого объедка gcnew
					btn->Size = Drawing::Size(50, 50); // Размер кнопки Size 
					btn->Location = Drawing::Point(col * 50, row * 50); //
					btn->Name = String::Format(L"button_{0}_{1}", row, col); // Уникальное имя для каждой кнопки, по строке и столбцу
					btn->Text = ""; //
					this->Controls->Add(btn);
					buttons[row, col] = btn;
				}
			}
		};




#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(411, 401);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	};
}
