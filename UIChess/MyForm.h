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
			CreateTurnLabel();
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
		bool whiteTurn = true; // Флаг для очередности
        /*----права рокировки----*/
		bool wKingMoved = false;
		bool bKingMoved = false;
		bool wRookAMoved = false;   
		bool wRookHMoved = false;   
		bool bRookAMoved = false;   
		bool bRookHMoved = false;
	
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
				{25,24,23,22,21,23,24,25}, // 2x - Черные, 1x - Белые
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

	


	public: bool isMoving = false;

	private: Label^ turnLabel; // для надписи

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
					btn->BackColor =
						((row + col) % 2 == 0)
						  ?Color::AntiqueWhite:   // светлая клетка
						  Color::SandyBrown;  // тёмная клетка

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


		/*-------------------------------
		*   Проверки легальности ходов 
		--------------------------------*/
		bool IsLegalPawnMove(int fromRow, int fromCol, int toRow, int toCol, int side) // ПЕШКИ
		{
			int dir = (side == 1) ? -1 : 1; // Определям направление движения пешки у белых -1 у чёрных +1
			int startRow = (side == 1) ? 6 : 1;  // Стартовая строка для пешек 6 у белых 1 у чёрных 

			if (fromCol == toCol) // Просто ход вперед без взятия
			{
				// Обычный шаг на одну клетку если поле пустое
				if (toRow == fromRow + dir && map[toRow, toCol] == 0) 
					return true;
				// Двойной шаг если обе клетки пусты
				if (fromRow == startRow && toRow == fromRow + 2 * dir &&
					map[fromRow + dir, fromCol] == 0 && map[toRow, toCol] == 0)
					return true;
			}
			// Взятие по диагонали 
			if (System::Math::Abs(toCol - fromCol) == 1 && toRow == fromRow + dir)
			{
				// Если стоит фигура по диагонали и пешка ходит по диагонали то это взятие
				if (map[toRow, toCol] != 0 && (map[toRow, toCol] / 10) != side)
					return true;
			}
			// В остальных случаях ход негелаен
			return false;
		}

		bool IsLegalKnightMove(int fromRow, int fromCol, int toRow, int toCol, int side) // КОНИ
		{
			int dir = toRow - fromRow;
			int dc = toCol - fromCol;
			// Проверяем Г-образный ход: (2,1) или (1,2)
			if ((System::Math::Abs(dir) == 2 && System::Math::Abs(dc) == 1) ||
				(System::Math::Abs(dir) == 1 && System::Math::Abs(dc) == 2))
			{
				// Целевая клетка должна быть либо пустой, либо занятой вражеской фигурой
				int dest = map[toRow, toCol];
				if (dest == 0 || (dest / 10) != side)
					return true;
			}
			return false;
		}

		bool IsLegalBishopMove(int fromRow, int fromCol, int toRow, int toCol, int side) // СЛОНЫ
		{
			int dr = toRow - fromRow;
			int dc = toCol - fromCol;
			if (System::Math::Abs(dr) != System::Math::Abs(dc) || dr == 0) // Диагональ |dr| = |dc| и ход не нулевой
				return false;
			int stepr = (dr > 0) ? 1 : -1; // Проверяем, что все клетки по маршруту пусты
			int stepc = (dc > 0) ? 1 : -1;
			int r = fromRow + stepr;
			int c = fromCol + stepc;
			while (r != toRow) {          // Цикл проверяет клетки по пути, кроме начальной и конечной
				if (map[r, c] != 0)     // Если встретилась любая фигура — движение заблокировано
					return false;
				r += stepr;
				c += stepc;
			}

			// Целевая клетка должна быть либо пустой, либо занятой вражеской фигурой
			int dest = map[toRow, toCol];
			return (dest == 0 || (dest / 10) != side);
		}
	
		bool IsLegalRookMove(int fromRow, int fromCol, int toRow, int toCol, int side) // ЛАДЬИ
		{
			// Ход должен быть ход строго по строке или по столбцу
			if (fromRow != toRow && fromCol != toCol) // Движение по диагонали - возвращаем false 
				return false;

			// Проверяем, что между from и to все клетки пусты
			int dr = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0; // движемся вниз dr → 1; вверх dr → -1; по горизонтали dr → 0, dc → +-1
			int dc = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;
			int r = fromRow + dr;
			int c = fromCol + dc;
			while (r != toRow || c != toCol) { // Цикл проверяет клетки по пути, кроме начальной и конечной
				if (map[r, c] != 0)  // Если встретилась любая фигура — движение заблокировано
					return false;
				r += dr; // Переход к некст клетке по направлению хода
				c += dc;
			}

		// Целевая клетка должна быть либо пустой, либо занятой вражеской фигурой
		int dest = map[toRow, toCol]; 
		if (dest == 0 || (dest / 10) != side)
			return true;

		return false;
		}

		bool IsLegalQueenMove(int fromRow, int fromCol, int toRow, int toCol, int side) // ФЕРЗЬ
		{
			return IsLegalRookMove(fromRow, fromCol, toRow, toCol, side) || IsLegalBishopMove(fromRow, fromCol, toRow, toCol, side); // Объединяем ладью и ферзя
		}

		bool IsLegalKingMove(int fromRow, int fromCol, int toRow, int toCol, int side, bool &isCastle) // КОРОЛЬ (& - ссылка, чтобы функция меняла значение передаваемой переменной)
		{
			isCastle = false;
			int dr = System::Math::Abs(toRow - fromRow);
			int dc = System::Math::Abs(toCol - fromCol);
			if ((dr <= 1 && dc <= 1) && (dr + dc > 0)) // Не дальше одной клетки в любом направлении, суммарный сдвиг больше 0. 
			{
				// Целевая клетка должна быть либо пустой, либо занятой вражеской фигурой
				int dest = map[toRow, toCol];
				if (dest == 0 || (dest / 10) != side)
					return true;
			}
			/* Попытка рокировки */
			if (dr == 0 && dc == 2 && fromRow == toRow)
			{
				if (toCol == 6 && CanCastleKingside(side)) { 
					isCastle = true;  
						return true; }
				if (toCol == 2 && CanCastleQueenside(side)) {
					isCastle = true;
						return true; }
			}
			return false;
			return false;
		}

		/*-----------------------------------------------
		*  Универсальная проверка атаки врага на клетку
		------------------------------------------------*/
		bool IsSquareUnderAttack(int row, int col, int bySide)
		{
			for (int r = 0; r < 8; ++r)
			{                                              // Двойной цикл по всем 64 клеткам полня
				for (int c = 0; c < 8; ++c)
				{
					int code = map[r, c]; // Получаем коды фигур 
					if (code == 0 || code / 10 != bySide) // Пропускаем пустые клетки и всё что не подходит нужной сторное
						continue;

					int piece = code % 10; // Получаем тип фигур
					bool attack = false; // Изначально считаем что ниче цель не атакует (флаг атаки)
					 
					switch (piece)  // Позволяет выбирать между несколькими разделами кода в зависимости от значения piece
					{
					case 6: // пешка
						attack = IsLegalPawnMove(r, c, row, col, bySide);
						break; // Избегаем дальнешей проверки если кейс верный
					case 4: // конь
						attack = IsLegalKnightMove(r, c, row, col, bySide);
						break;
					case 5: // ладья
						attack = IsLegalRookMove(r, c, row, col, bySide);
						break;
					case 3: // слон
						attack = IsLegalBishopMove(r, c, row, col, bySide);
						break;
					case 2: // ферзь
						attack = IsLegalQueenMove(r, c, row, col, bySide);
						break;
					case 1: // король
						bool castleFlag = false;
						attack = IsLegalKingMove(r, c, row, col, bySide, castleFlag);
						break;
					}

					if (attack) // Если для какой то фигуры attack = true → клетка под ударом
						return true;
				} 
			}
			return false; // Если для какой то фигуры attack = false → клетка не под ударом
		}

		void UpdateTurnLabel()                    
		{
			turnLabel->Text = whiteTurn ? "Ход белых" : "Ход черных";
		}

		void MyForm::CreateTurnLabel()
		{
			turnLabel = gcnew Label();
			turnLabel->AutoSize = true;
			turnLabel->Font = gcnew Drawing::Font("Calibri light", 12);
			turnLabel->Location = Drawing::Point(450, 10);
			this->Controls->Add(turnLabel);
			UpdateTurnLabel();
		}

		/*--------------------------
		  Функция обновления флагов
		----------------------------*/
		void UpdateCastleFlags(int fromRow, int fromCol, int toRow, int toCol, int code, int captured)
		{
			int side = code / 10; // Определяем сторону
			int piece = code % 10; // Определям тип фигуры

			if (piece == 1) // Если ходил король
			{
				if (side == 1) wKingMoved = true; // белый ходил
				else           bKingMoved = true; // черный ходил
			}
			if (piece == 5) // Если ходила ладья
			{
				if (side == 1 && fromRow == 7) // Для белых ладья стартовала на 7 строке
				{
					if (fromCol == 0) wRookAMoved = true;  // a1 - col0; h1 - col 7
					if (fromCol == 7) wRookHMoved = true;
				}
				if (side == 2 && fromRow == 0) // Для черных на нулевой строке
				{
					if (fromCol == 0) bRookAMoved = true; // a8 
					if (fromCol == 7) bRookHMoved = true; // h8
				}
			}
			if (captured != 0 && captured % 10 == 5) // Если взяли ладью
			{
				int cSide = captured / 10; // Определяем сторону съеденной ладьи
				if (cSide == 1 && toRow == 7) // Если съели белую ладью на её исходной 7 строке
				{
					if (toCol == 0) wRookAMoved = true; // меняем флаги
					if (toCol == 7) wRookHMoved = true; 
				}
				if (cSide == 2 && toRow == 0) // Если съели чёрную ладью на её исходной 0 строке
				{
					if (toCol == 0) bRookAMoved = true;
					if (toCol == 7) bRookHMoved = true;
				}
			}
		}

		/*-------------------------------
		 Функция, выполняющая рокировку
		-------------------------------*/
		void doCastle(int side, bool kingside)
		{
			if (side == 1)         // белые (row 7)
			{
				if (kingside)      // 0-0
				{
					map[7, 6] = 11;   // король на g1
					map[7, 5] = 15;   // ладья на f1
					map[7, 4] = 0;
					map[7, 7] = 0;
					wKingMoved = wRookHMoved = true;
				}
				else               // 0-0-0
				{
					map[7, 2] = 11;   // король на c1
					map[7, 3] = 15;   // ладья на d1
					map[7, 4] = 0;
					map[7, 0] = 0;
					wKingMoved = wRookAMoved = true;
				}
			}
			else // чёрные (row 0)
			{
				if (kingside)
				{
					map[0, 6] = 21;
					map[0, 5] = 25;
					map[0, 4] = 0;
					map[0, 7] = 0;
					bKingMoved = bRookHMoved = true;
				}
				else
				{
					map[0, 2] = 21;
					map[0, 3] = 25;
					map[0, 4] = 0;
					map[0, 0] = 0;
					bKingMoved = bRookAMoved = true;
				}
			}
		}

		/*--------------
		  Проверка шаха
		----------------*/
		bool IsKingInCheck(int side)
		{
			// Находим координаты своего короля
			int kRow = -1, kCol = -1;
			int kingCode = side * 10 + 1;
			for (int r = 0; r < 8 && kRow < 0; ++r)
				for (int c = 0; c < 8; ++c)
					if (map[r, c] == kingCode)
					{
						kRow = r; kCol = c;
						break;
					}
			if (kRow < 0) return false;  // короля нет

			// Смотрим может ли его атаковать противник
			int enemy;
			if (side == 1)
				enemy = 2;
			else 
				enemy = 1;
			return IsSquareUnderAttack(kRow, kCol, enemy); 
		}


		/*-------------------------
		*  Проверка прав рокировки
		--------------------------*/
		bool CanCastleKingside(int side)   // 0-0
		{
			if (side == 1) // Белые
				return !wKingMoved && !wRookHMoved && // Флаги должны быть false
				map[7, 5] == 0 && map[7, 6] == 0 && // [7, 4] - e1; [7, 5] - f1; [7, 6] - g1
				!IsSquareUnderAttack(7, 4, 2) && // Проверяем все эти клетки на атаки, передаем в функцию IsSquareUnderAttack координаты и сторону
				!IsSquareUnderAttack(7, 5, 2) &&
				!IsSquareUnderAttack(7, 6, 2);
			else // Черные
				return !bKingMoved && !bRookHMoved && 
				map[0, 5] == 0 && map[0, 6] == 0 && // Аналогично с белыми
				!IsSquareUnderAttack(0, 4, 1) &&
				!IsSquareUnderAttack(0, 5, 1) &&
				!IsSquareUnderAttack(0, 6, 1);
		}

		bool CanCastleQueenside(int side)  // 0-0-0
		{
			if (side == 1) // Белые
				return !wKingMoved && !wRookAMoved && // Флаги должны быть false
				map[7, 1] == 0 && map[7, 2] == 0 && map[7, 3] == 0 && // [7, 3] — d1; [7, 2] — c1; [7, 1] — b1
				!IsSquareUnderAttack(7, 4, 2) && 
				!IsSquareUnderAttack(7, 3, 2) &&
				!IsSquareUnderAttack(7, 2, 2);
			else // Черные
				return !bKingMoved && !bRookAMoved &&
				map[0, 1] == 0 && map[0, 2] == 0 && map[0, 3] == 0 &&
				!IsSquareUnderAttack(0, 4, 1) &&
				!IsSquareUnderAttack(0, 3, 1) &&
				!IsSquareUnderAttack(0, 2, 1);
		}


		/*------ КЛИКИ ИВЕНТЫ -------*/
	private: System::Void OnCellClick(System::Object^ sender, System::EventArgs^ e) { // Обработчик клика по ячейке шахматной доски
		Button^ btn = (Button^)sender; // Получаем кнопку, по которой был клик (приведение sender к типу button) 
		// Парсим имя, чтобы узнать координаты
		int row, col;
		String^ name = btn->Name; // "button_{row}_{col}" например button_2_3
		array<String^>^ parts = name->Split('_'); 
		row = Int32::Parse(parts[1]); // Конвертируем вторую часть в число — номер строки //Преобразует строковое представление числа в эквивалентное ему 32-битовое целое число со знаком.
		col = Int32::Parse(parts[2]); // Третью - номер столбцы

		// Если ничего не выбрано
		if (selectedRow == -1 && selectedCol == -1) {
			if (map[row, col] != 0) { // Проверяем, есть ли фигура в этой клетке
				int side = map[row, col] / 10;
				if ((whiteTurn && side == 1) || (!whiteTurn && side == 2)) {
					// Клик по своей фигуре - берем ее в руку
					selectedRow = row;
					selectedCol = col;
					buttons[row, col]->FlatStyle = FlatStyle::Flat; // Меняем стиль кнопки
					buttons[row, col]->FlatAppearance->BorderColor = Color::Red; // Красная рамка
					buttons[row, col]->FlatAppearance->BorderSize = 2; // Размер выделения границы
				}
			}
		}
		else { // Если уже выбрана фигура, то текущий клик — это попытка сделать ход
			int code = map[selectedRow, selectedCol]; // Получаем код выбранной фигуры
			int side = code / 10; // Сторона (1 или 2)
			int piece = code % 10; // Тип фигуры 
			bool legal = true; // Флаг легальности

			if (piece == 6) { // Если фигура пешка, то вызываем 
				legal = IsLegalPawnMove(selectedRow, selectedCol, row, col, side);
			} 
			else if (piece == 4) { // Если фигура конь, то вызываем
				legal = IsLegalKnightMove(selectedRow, selectedCol, row, col, side);
			}
			else if (piece == 5) { // Если фигура ладья, то вызываем
				legal = IsLegalRookMove(selectedRow, selectedCol, row, col, side);
			}
			else if (piece == 3) {                  // Если фигура слон, то вызываем
				legal = IsLegalBishopMove(selectedRow, selectedCol, row, col, side);
			}
			else if (piece == 2) {                  // Если фигура ферзь, то вызываем
				legal = IsLegalQueenMove(selectedRow, selectedCol, row, col, side);
			}
			else if (piece == 1) // Если фигура король, то вызываем
			{
				bool isCastle = false;
				legal = IsLegalKingMove(selectedRow, selectedCol, row, col, side, isCastle);

				if (legal && isCastle) // условие легальности и если ниче не двигалось
				{
					/* Выполнение рокировки*/					 
					bool kingside = (col == 6); 
					doCastle(side, kingside);
					UpdateBoard();
					whiteTurn = !whiteTurn;
					UpdateTurnLabel();
					selectedRow = selectedCol = -1;
					return;                      
				}
			}
			// Сюда добавлю остальные IsLegalPieceTypeMove

			if (legal && ((map[row, col] == 0) || (map[row, col] / 10) != side)) {
				int captured = map[row, col]; // СОХРАНЯЕМ ЖЕРТВУ
				map[row, col] = code; // Перемещаем код фигуры в новую клетку
				UpdateCastleFlags(selectedRow, selectedCol, row, col, code, captured);
				map[selectedRow, selectedCol] = 0; // Очищаем старую клетку

				if (IsKingInCheck(side)) { // Проверка шаха для своей стороны
					map[selectedRow, selectedCol] = code; // Возвращаем короля 
					map[row, col] = captured; // Возвращаем жертву
					MessageBox::Show("Невозможный ход, шах.", "ошибка");
				}
				else {
					whiteTurn = !whiteTurn; // Ход легален, И ТОЛЬКО В ЭТОМ СЛУЧАЕ МЕНЯЕМ СТОРОНУ
					UpdateTurnLabel();
					if (IsKingInCheck(whiteTurn ? 2 : 1)) {
						MessageBox::Show("Шах!", "ошибка");
					}
				}

			}



			// Сбрасываем выделение
			UpdateBoard();
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
			this->ClientSize = System::Drawing::Size(550, 401);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	};
}
