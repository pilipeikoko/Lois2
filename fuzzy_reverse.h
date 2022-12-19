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

#include "fuzzy_logic.h"

using namespace std;

using input_data = vector<pair<set, impl_matrix>>;

inline arr parse_arr_of_matrix(string line) {
	string delimiter = "    ";
	size_t pos = 0;
	float token;
	arr array;
	while ((pos = line.find(delimiter)) != string::npos) {
		string str_token = line.substr(0, pos);
		token = stof(str_token);
		array.push_back(make_pair("", token));
		line.erase(0, pos + delimiter.length());
	}
	if (line.length() >= 1)
	{
		token = stof(line);
		array.push_back(make_pair("", token));
	}
	return array;
}

inline input_data parse_input_for_reverse(vector<string> lines) {
	bool next_is_matrix = false;
	input_data data;
	impl_matrix current_matrix;
	set current_set;
	for (size_t i = 0; i < lines.size(); i++)
	{
		string line = lines[i];
		if (line.find_first_of("#") == 0)
		{
			next_is_matrix = false;
			if (current_set.second.size() != 0)
			{
				data.push_back(make_pair(current_set, current_matrix));
				current_matrix.clear();
			}
		}
		else if (line.find("=") != -1)
		{
			current_set = parse_set_out_of_string(line);
			next_is_matrix = true;
		}
		else if (next_is_matrix)
		{
			arr array = parse_arr_of_matrix(line);
			current_matrix.push_back(array);
		}
	}
	if (current_matrix.size() != 0)
	{
		data.push_back(make_pair(current_set, current_matrix));
	}
	return data;
}

inline void show_reverse_input(input_data data) {
	for (int i = 0; i < data.size(); i++)
	{
		show_set(data[i].first);
		show_impl_matrix(data[i].second, to_string(i));
	}
}

inline void show_reverse_input(input_data data, int choosen) {
	cout << "You choose: " << endl;
	show_set(data[choosen].first);
	show_impl_matrix(data[choosen].second, to_string(choosen));
}

vector<vector<pair<string, float>>> compute_reverse_composition(arr array, impl_matrix matrix) {
	vector<vector<pair<string, float>>> solutions;
	for (size_t i = 0; i < matrix.size(); i++)
	{
		vector<pair<string, float>> solution;
		for (size_t j = 0; j < matrix[0].size(); j++)
		{
			float a = -1;
			string sign;
			if (array[j].second == matrix[i][j].second)
			{
				a = matrix[i][j].second;
				sign = "more";
			}
			else if (array[j].second < matrix[i][j].second)
			{
				a = array[j].second;
				sign = "equals";
			}
			solution.push_back({ sign, a });
		}
		solutions.push_back(solution);
	}
	return solutions;
}

vector<pair<float, float>> compute_actual_solution(vector<vector<pair<string, float>>> solutions) {
	vector<pair<float, float>> actual_sltn;
	for (size_t i = 0; i < solutions.size(); i++)
	{
		float current_more = 0;
		float current_less = 1;
		for (size_t j = 0; j < solutions[0].size(); j++)
		{
			float current_a = solutions[i][j].second;
			string sign = solutions[i][j].first;
			if (sign == "more")
			{
				if (current_a > current_more)
				{
					current_more = current_a;
				}
			}
			else if (sign == "equals")
			{
				bool is_only_solution = true;
				for (size_t k = 0; k < i; k++)
				{
					if (solutions[k][j].first == "more")
					{
						is_only_solution = false;
					}
				}
				if (is_only_solution)
				{
					current_less = current_a;
					current_more = current_a;
				}
				else if (current_a < current_less)
				{
					current_less = current_a;
					if (current_more > current_less)
					{
						current_more = 0;
					}
				}
			}
		}
		actual_sltn.push_back({ current_more, current_less });
	}
	return actual_sltn;
}

void print_solutions(set i_set, int m_count, vector<pair<float, float>> solution) {
	int i = 0;
	cout << "Solution:";
	cout << "(";
	while (i < solution.size())
	{
		cout << i_set.first << m_count << "(x" << i + 1 << ")";
		if (i + 1 != solution.size())
		{
			cout << ",";
		}
		i++;
	}
	cout << ")E";

	cout << "(";
	for (size_t j = 0; j < solution.size(); j++)
	{
		cout << "[";
		if (solution[j].first == solution[j].second)
		{
			cout << solution[j].first;
		}
		else
		{
			cout << solution[j].first << "," << solution[j].second;
		}
		cout << "]";
		if (j + 1 != solution.size())
		{
			cout << "x";
		}
	}
	cout << ")" << endl;

}

void print_no_solution(set i_set, int m_count) {
	cout << i_set.first << m_count << " has no solutions " << endl;
}

bool has_solution(arr array, impl_matrix matrix) {
	std::vector<int> temp_vector(matrix[0].size(), 0);

	for (size_t i = 0; i < matrix.size(); i++)
	{
		for (size_t j = 0; j < matrix[0].size(); j++)
		{
			if (array[j].second > matrix[i][j].second)
			{
				temp_vector[j]++;
			}
		}
	}

	for (int i = 0; i < temp_vector.size(); ++i) {
		if (temp_vector[i] == matrix.size()) {
			return false;
		}
	}

	return true;
}