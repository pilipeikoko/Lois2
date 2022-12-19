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

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <algorithm>

using namespace std;

using arr = vector<pair<string, float>>;
using set = pair<string, arr>;
using fuzzy_sets = vector<set>;
using impl_matrix = vector<arr>;

inline void show_set(set show) {
	cout << show.first << "={";
	for (size_t j = 0; j < show.second.size(); j++)
	{
		if (j != 0)
		{
			cout << ",";
		}
		cout << "(" << show.second[j].first << ",";
		cout << show.second[j].second << ")";
	}
	cout << "}" << endl;
}

inline void valid_substring(string str) {
	if (str.empty() || str.find(')') != string::npos || str.find_last_of('(') != 0 || str.find_first_of(',') != str.find_last_of(','))
	{
		throw exception("");
	}
}

inline void valid_arr_val(float arr_val) {
	if (arr_val < 0 || arr_val > 1)
	{
		throw exception("");
	}
}


inline arr parse_arr_out_of_string(string str) {
	str = str.substr(str.find('=') + 2);
	arr temp_arr;
	while (!str.empty())
	{
		const size_t start = str.find_first_of('(');
		const size_t end = str.find_first_of(')');
		const size_t middle_comma = str.find_first_of(',');

		string temp = str.substr(start, end);
		valid_substring(temp);
		string arr_var = temp.substr(start + 1, middle_comma - start - 1);
		float arr_val = stof(temp.substr(middle_comma + 1, end - middle_comma));
		valid_arr_val(arr_val);
		pair<string, float> temp_pair = make_pair(arr_var, arr_val);
		temp_arr.push_back(temp_pair);
		str = str.substr(end + 2);
	}
	return temp_arr;
}

inline set parse_set_out_of_string(string str) {
	const size_t i = str.find('=');
	string set_name = str.substr(0, 1);
	arr temp_arr = parse_arr_out_of_string(str);
	set parsed_set(set_name, temp_arr);
	return parsed_set;
}

inline void show_impl_matrix(impl_matrix matrix, string num) {
	cout << "Matrix with index " << num << ":" << endl;
	for (size_t i = 0; i < matrix.size(); i++)
	{
		for (size_t j = 0; j < matrix[i].size(); j++)
		{
			string var = matrix[i][j].first;
			float val = matrix[i][j].second;
			if (var != "")
			{
				cout << var << ": ";
			}
			cout << val << "\t";
		}
		cout << endl;
	}
	cout << endl;
	cout << endl;
}