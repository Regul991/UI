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


	/*----Переменнные для хранения----*/
	private:
		int selectedRow = -1;
		int selectedCol = -1;
		bool whiteTurn = true;
	
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

	
	private:
		bool whiteTurn = true; // Флаг для очередности хода

	public: bool isMoving = false;
	/*-------------------------------------------
		 Создаем массив-доску из 64 кнопок
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
					btn->Click += gcnew System::EventHandler(this, &MyForm::OnCellClick);

				}
			}
		}

		/*------ КЛИКИ ИВЕНТЫ -------*/
	private: System::Void OnCellClick(System::Object^ sender, System::EventArgs^ e) { // Обработчик клика по ячейке шахматной доски
		Button^ btn = (Button^)sender; // Получаем кнопку, по которой был клик
		// Парсим имя, чтобы узнать координаты
		int row, col;
		String^ name = btn->Name; // "button_{row}_{col}" например button_2_3
		array<String^>^ parts = name->Split('_'); 
		row = Int32::Parse(parts[1]); // Конвертируем вторую часть в число — номер строки //Преобразует строковое представление числа в эквивалентное ему 32-битовое целое число со знаком.
		col = Int32::Parse(parts[2]); // Третью - номер столбцы

		// Если ничего не выбрано
		if (selectedRow == -1 && selectedCol == -1) {
			if (map[row, col] != 0) { // Проверяем, есть ли фигура в этой клетке
				// Клик по своей фигуре - берем ее в руку
				selectedRow = row;
				selectedCol = col;
				buttons[row, col]->FlatStyle = FlatStyle::Flat; // Меняем стиль кнопки
				buttons[row, col]->FlatAppearance->BorderColor = Color::Red; // Красная рамка
				buttons[row, col]->FlatAppearance->BorderSize = 2; // Размер выделения границы?? хз как описать 
			}
		}
		else {
			// Пытаемся походить (пока что без легальности)
			map[row, col] = map[selectedRow, selectedCol]; // Перемещаем код фигуры в новую клетку
			map[selectedRow, selectedCol] = 0; // Очищаем старую клетку
			UpdateBoard(); // Обновляем доску она снизу
			// Сброс выделения
			buttons[selectedRow, selectedCol]->FlatStyle = FlatStyle::Standard;
			selectedRow = -1;
			selectedCol = -1;
		}
	}
		   /*---------- Обновление ----------*/ /*Метод для перерисовки всех кнопок на доске по состоянию массива map*/
		   void UpdateBoard()
		   {
			   for (int row = 0; row < 8; row++) // Проходим по всем строкам
			   {
				   for (int col = 0; col < 8; col++) // Проходим по всем столбцам
				   {
					   Button^ btn = buttons[row, col]; // Выбираем кнопку на позиции [row,col]
					   int code = map[row, col]; // Ну тут в целом все как в Create Buttons
					   btn->BackgroundImage = nullptr;
					   if (code != 0)
					   {
						   int side = code / 10;
						   int piece = code % 10;
						   Bitmap^ bmp = gcnew Bitmap(50, 50);
						   Graphics^ g = Graphics::FromImage(bmp);
						   int srcX = 150 * (piece - 1);
						   int srcY = (side == 1) ? 0 : 150;
						   g->DrawImage(chessSprites,
							   System::Drawing::Rectangle(0, 0, 50, 50),
							   srcX, srcY, 150, 150,
							   GraphicsUnit::Pixel);
						   btn->BackgroundImage = bmp;
						   btn->BackgroundImageLayout = ImageLayout::Stretch;
					   }
				   }
			   }
		   }
		   /*------------------------------------------------
		   * Почему нужен UpdateBoard() отдельно?           *
		   * Потому что пересоздавать кнопки снова медленно *
		   * Неудобно                                       *
		   * И ненужно ведь сетка не меняется               *
		   -------------------------------------------------*/





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
