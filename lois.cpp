//////////////////////////////////////////////////////////////////////////////////////
// Лабораторная работа 2 по дисциплине ЛОИС
// Выполнена студентами группы 921701 БГУИР
// Пилипейко В.И. и Драгун В.А. - разработали программу в которой отображён алгоритм
// Василевский А.Д. и Кульбеда И.А. - произвели анализ и отобразили результат это в отчёте
// Вариант 6 - Реализовать обратный нечеткий логический вывод на основе операции нечеткой композиции (max{min({x_i}U{y_i})|i})
// 12.12.2022
// Использованные материалы:
// "Нечеткий логический вывод в системе принятия решений" А. А. Ахрем, М. Р. Ашинянц. С. А. Петров
// "Логические основы интеллектуальных систем. Практикум" В. В. Голенков, В. П. Ивашенко, Д. Г. Колб, К. А. Уваров
// https://youtu.be/eMXlvVcXU9I - Нечеткие множества (Fuzzy Sets)
// https://en.wikipedia.org/wiki/Construction_of_t-norms -T-norm
// https://en.wikipedia.org/wiki/T-norm#Properties_of_t-norms - Construction of t-norms
// https://drive.google.com/drive/folders/1OLTrp892NW2hVS77H3AvhprqBgBcCJva - Лоис 2 конспект

#include <Windows.h>  
#include<sstream> 
#include "resource.h"
#include "fuzzy_reverse.h"

HMODULE GCM()
{
	HMODULE h_module = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GCM,
		&h_module);
	return h_module;
}

int len(string str)
{
	int length = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		length++;

	}
	return length;
}

vector<string> split(string str, char seperator)
{
	vector<string> strings;
	int currIndex = 0, i = 0;
	int startIndex = 0, endIndex = 0;
	while (i <= len(str))
	{
		if (str[i] == seperator || i == len(str))
		{
			endIndex = i;
			string subStr = "";
			subStr.append(str, startIndex, endIndex - startIndex);
			if (subStr.size() > 0)
			{
				strings.push_back(subStr);
			}
			currIndex += 1;
			startIndex = endIndex + 1;
		}
		i++;
	}
	return strings;
}

vector<string> parse_strings_from_file(int demo_text, int text_file) {
	string result;
	HRSRC h_res = FindResource(GCM(), MAKEINTRESOURCE(demo_text), MAKEINTRESOURCE(text_file));
	HGLOBAL h_data = LoadResource(GCM(), h_res);
	DWORD h_size = SizeofResource(GCM(), h_res);
	char* h_final = (char*)LockResource(h_data);
	result.assign(h_final, h_size);
	return split(result, '\n');
}


bool should_pick_next() {
	cout << "Please enter 'exit' to exit or any value to continue:\n";
	string s;
	cin >> s;
	if (s == "") {
		return false;
	}
	system("cls");
	return true;
}

int get_matrix_number() {
	cout << "Please enter number of matrix:\n";
	int chosen_index;
	cin >> chosen_index;
	return chosen_index;
}

void reverse_output() {
	vector<string> input_file_lines = parse_strings_from_file(DEMO_TEXT2, TEXTFILE2);
	input_data input_data = parse_input_for_reverse(input_file_lines);

	while (true)
	{
		show_reverse_input(input_data);

		int selected_index = get_matrix_number();

		show_reverse_input(input_data, selected_index);

		set set = input_data[selected_index].first;
		impl_matrix matrix = input_data[selected_index].second;

		if (!has_solution(set.second, matrix)) {
			print_no_solution(set, selected_index);

			if (should_pick_next()) {
				continue;
			}
			else {
				break;
			}
		}

		// вычисление интервалов для каждого уравнения системы
		vector<vector<vector<pair<float, float>>>> solutions = compute_reverse_composition(set.second, matrix);
		// вычисление общего решения
		vector<vector<pair<float, float>>> actual_solution = compute_actual_solution(solutions);

		print_solutions(set, selected_index, actual_solution);

		if (!should_pick_next()) {
			break;
		}
	}
}

int main()
{
	reverse_output();

	return 0;
}