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
			InitMap();
			chessSprites = gcnew Bitmap(
				L"Z:\\pp\\UI\\UIChess\\UI\\UIChess\\chess.png"
			);		// Я не понимаю...литералы какие то 
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

	private:
		// модель доски: код = десятки (сторона), единицы (тип фигуры)
		cli::array<int, 2>^ map;
		// картинка со всеми спрайтами
		Bitmap^ chessSprites;
		void InitMap()
		{
			static int initial[8][8] = {
				{25,24,23,22,21,23,24,25}, // 1x - Черные, 2x - Белые
				{26,26,26,26,26,26,26,26},
				{ 0, 0, 0, 0, 0, 0, 0, 0},
				{ 0, 0, 0, 0, 0, 0, 0, 0},
				{ 0, 0, 0, 0, 0, 0, 0, 0},
				{ 0, 0, 0, 0, 0, 0, 0, 0},
				{16,16,16,16,16,16,16,16},
				{15,14,13,12,11,13,14,15}
			};
			map = gcnew cli::array<int, 2>(8, 8);
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					map[i, j] = initial[i][j];
		}


	
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
					btn->Text = ""; // Пока что пустой текст
					this->Controls->Add(btn); 
					buttons[row, col] = btn; // Ссылка на кнопку для дальнейшей логики

					int code = map[row, col]; // Получаем код фигуры из карты сверху 
					if (code != 0) 
					{
						int side = code / 10;         // 1 = белые сверху, 2 = чёрные снизу сторона Side
						int piece = code % 10;        // 1=король,2=ферзь,3=слон,5=ладья,6=пешка,4=конь тип Piece
						Bitmap^ bmp = gcnew Bitmap(50, 50); // создаем юитмап 50 на 50 пикселей 
						Graphics^ g = Graphics::FromImage(bmp); // получаем объект Graphics для рисования 
						int srcX = 150 * (piece - 1); // Смещение по x на спрайт листе (одна фигура занимает 150 пикселей) 
						int srcY = (side == 1) ? 0 : 150; // Смещение по y, белые на верхней стороне картинки, чёрные на нижей (см. chess.png)
						g->DrawImage(chessSprites,
							System::Drawing::Rectangle(0, 0, 50, 50), // Рисуем в весь прямоугольник 50 на 50 (размер button) 
							srcX, srcY, 150, 150, 
							GraphicsUnit::Pixel); 
						btn->BackgroundImage = bmp; // Устанавливаем как фон
						btn->BackgroundImageLayout = ImageLayout::Stretch; // Меняем tile на stretch чтобы изображение не повторялось внутри кнопки
					}

				}
			}
		}






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
