#include <iostream>
#include <fstream> 
#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>


using namespace std;
HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

// Цвета консоли
enum ConsoleColor {
	LightGray = 7,
	LightGreen = 10,
};

class CSV
{
public:
	void readNextRow(std::istream& str)
	{
		string line;
		getline(str, line);

		stringstream lineStream(line);
		string cell;
		if (!line.empty()) {
			rowCVS.clear();
		}
		while (getline(lineStream, cell, ';'))
		{
			rowCVS.push_back(cell);
		}
		if (!lineStream && cell.empty())
		{
			rowCVS.push_back("");
		}
		tabelCVS.push_back(rowCVS);
	}

	int SearchingArgInVector(string& arg) {
		int index = tabelCVS.size();
		int checkingForTheArgOfMatches = 0;
		for (size_t i = 0; i < rowCVS.size() - 1; i++)
		{
			if (arg == tabelCVS[0][i]) {
				index = i;
				checkingForTheArgOfMatches++;
			}
		}
		if (checkingForTheArgOfMatches > 1) {
			throw "Several identical arguments were found";
		}
		return index;
	}

	int SearchingNumInVector(string& num) {
		int index = tabelCVS.size();
		int checkingForTheNumberOfMatches = 0;
		for (size_t i = 0; i < tabelCVS.size() - 1; i++)
		{
			if (num == tabelCVS[i][0]) {
				index = i;
				checkingForTheNumberOfMatches++;
			}
		}
		if (checkingForTheNumberOfMatches > 1) {
			throw "Several identical numbers were found";
		}
		return index;
	}
	string SearchingAndCalculate(string& arg1, string& num1, string& arg2, string& num2, string& sign) {
		
		string result = "";
		
		int indexArg1 = SearchingArgInVector(arg1);
		int indexArg2 = SearchingArgInVector(arg2);

		int indexNum1 = SearchingNumInVector(num1);
		int indexNum2 = SearchingNumInVector(num2);
		//auto n = find(tabelCVS[0].begin(), tabelCVS[0].end(), arg1); // почему-то компилятору MinGW это не понравилось
		//int index1 = n - tabelCVS[0].begin();

		//n = find(tabelCVS[0].begin(), tabelCVS[0].end(), arg2);
		//int index2 = n - tabelCVS[0].begin();

		// проверка на наличие аргумента в списке
		if (indexArg1 >= tabelCVS.size() - 1 || indexArg2 >= tabelCVS.size() - 1) {
			throw "The argument name was not found";
		}
		// проверка на наличие номера в списке
		if (indexNum1 >= tabelCVS.size() - 1 || indexNum2 >= tabelCVS.size() - 1) {
			throw "The number was not found";
		}

		int numCell1 = atoi(tabelCVS[indexNum1][indexArg1].c_str());
		int numCell2 = atoi(tabelCVS[indexNum2][indexArg2].c_str());

		if (sign[0] == '+') {
			result = to_string(numCell1 + numCell2);
		}
		else if (sign[0] == '*') {
			result = to_string(numCell1 * numCell2);
		}
		else if (sign[0] == '/') {
			result = to_string(numCell1 / numCell2);
		}
		else if (sign[0] == '-') {
			result = to_string(numCell1 - numCell2);
		}
		return result;
		
		
		
		
		
	}

	string CheckingCell(string& cell) {
		
		if (cell[0] == '=') { // todo надо подумать как сделать для выражений состоящих из 3,4..n аргументов. И продумать порядок вычисления.
			SetConsoleTextAttribute(hConsoleHandle, LightGreen); // изменяю цвет, чтобы было видно место формулы
			string arg1 = "";
			string arg2 = "";
			string num1 = "";
			string num2 = "";
			string sign = "";

			for (size_t i = 1; i < cell.size(); i++)
			{

				if (cell[i] == '+' || cell[i] == '*' || cell[i] == '/' || cell[i] == '-') {
					sign = cell[i];
					i++;
					for (size_t j = i; j < cell.size(); j++)
					{
						if (!isdigit(cell[j])) {
							arg2 += cell[j];
						}
						else {
							num2 += cell[j];
						}
					}
					break;
				}
				else if (!isdigit(cell[i])) {
					arg1 += cell[i];
				}
				else {
					num1 += cell[i];
				}

			}
			return SearchingAndCalculate(arg1, num1, arg2, num2, sign);
		}
		else if (cell == "") {
			throw "There is no data in the cell";
		}
		else {
			SetConsoleTextAttribute(hConsoleHandle, LightGray); // возвращаю исходный цвет
			return cell;
		}

		return cell;
	}

	vector<string> rowCVS; //todo сделать приватным и перегрузить оператор []
	vector<vector<string>> tabelCVS; //todo сделать приватным и перегрузить оператор [] и оператор []
};

int main(int argc, char* argv[]) // Вход в программу 
{

	if (argc < 2) { // Проверка на ввод пути
		cout << "Usage: " << std::endl;
		cout << "Script filename.cvs" << std::endl;
		return 1;
	}
	try {
		ifstream in(argv[1], ios::in); // открываем файл
		//ifstream in("testSMALL.csv", ios::in); // для тестирования. Также следует изменить проверку на количество аргументов выше
		if (!in)
			throw "Can't open file"; // Если ввели файл не с тем разрешением
		CSV row;
		while (!in.eof())
		{
			row.readNextRow(in); // заполнение вектора-таблицы
		}


		for (size_t i = 0; i < row.rowCVS.size() - 1; i++) // вывод шапки. сделано отдельно, т.к. в шапке может быть пустая ячейка, а в таблице нет
		{
			cout << row.tabelCVS[0][i] + '\t';
		}
		cout << endl;



		for (size_t i = 1; i < row.tabelCVS.size() - 1; i++) // вывод таблицы 
		{
			for (size_t j = 0; j < row.rowCVS.size() - 1; j++)
			{
				row.tabelCVS[i][j] = row.CheckingCell(row.tabelCVS[i][j]); // проверка на формулы
				cout << row.tabelCVS[i][j] + '\t';
			}
			cout << endl;
		}
	}
	catch (const char *s) {
		cerr << endl << "ERROR: " << s << endl;
	}
	SetConsoleTextAttribute(hConsoleHandle, LightGray); // возвращаю цвет в исходный на всякий пожарный
	return 0;
}

