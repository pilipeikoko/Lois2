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

using arr = vector<pair<string, float>>;
using set = pair<string, arr>;
using fuzzy_sets = vector<set>;
using impl_matrix = vector<arr>;
using input_data = vector<pair<set, impl_matrix>>;

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

vector<vector<vector<pair<float, float>>>> compute_reverse_composition(arr array, impl_matrix matrix) {
	vector<vector<vector<pair<float, float>>>> solutions;

	for (size_t j = 0; j < matrix[0].size(); j++)
	{
		vector<vector<pair<float, float>>> solution_for_variable;
		for (size_t k = 0; k < matrix.size(); k++)
		{
			vector<pair<float, float>> solution;

			bool j_column_has_solution = matrix[k][j].second >= array[j].second;
			if (!j_column_has_solution) {
				continue;
			}
			float lower = 0;
			float higher = 0;

			if (matrix[k][j].second == array[j].second) {
				lower = array[j].second;
				higher = 1;
			}
			else if (matrix[k][j].second > array[j].second) {
				lower = array[j].second;
				higher = array[j].second;
			}

			for (int t = 0; t < matrix.size(); t++) {
				if (t == k) {
					solution.push_back({ lower, higher });
				}
				else {
					float min = matrix[t][j].second >= array[j].second ? array[j].second : 1;
					solution.push_back({ 0 , min });
				}
			}
			solution_for_variable.push_back(solution);
		}

		solutions.push_back(solution_for_variable);
	}

	return solutions;
}

vector<vector<pair<float, float>>> compute_actual_solution(vector<vector<vector<pair<float, float>>>> solutions) {
	vector<vector<pair<float, float>>> actual_sltn;

	for (int j = 0; j < solutions[0].size(); j++) {
		for (int k = 0; k < solutions[0].size(); k++) {
			vector<pair<float, float>> solution_for_column;
			for (int t = 0; t < solutions[j][k].size(); t++) {
				// J K T are intervals that are being checked
				float lower = solutions[j][k][t].first;
				float higher = solutions[j][k][t].second;
				int solution_counter = 0;
				for (int i = 1; i < solutions.size(); ++i) {
					for (int s = 0; s < solutions[i].size(); ++s) {
						float lower_to_be_compared = solutions[i][s][t].first;
						float higher_to_be_compared = solutions[i][s][t].second;

						if (lower_to_be_compared >= higher || lower <= higher_to_be_compared) {
							if (lower < lower_to_be_compared) {
								lower = lower_to_be_compared;
							}

							if (higher > higher_to_be_compared) {
								higher = higher_to_be_compared;
							}

							solution_counter++;
							break;
						}
					}
				}
				if (solution_counter == solutions.size() - 1) {
					solution_for_column.push_back({ lower,higher });
				}
			}
			if (solution_for_column.size() == solutions[j][k].size()) {
				actual_sltn.push_back(solution_for_column);
			}
		}
		break;
	}

	return actual_sltn;
}

void print_solutions(set i_set, int m_count, vector<vector<pair<float, float>>> merged_solutions) {
	int i = 0;
	cout << "Solution:";
	cout << "(";
	while (i < merged_solutions[0].size())
	{
		cout << i_set.first << m_count << "(x" << i + 1 << ")";
		if (i + 1 != merged_solutions[0].size())
		{
			cout << ",";
		}
		i++;
	}
	cout << ")E";
	for (size_t i = 0; i < merged_solutions.size(); i++)
	{
		cout << "(";
		for (size_t j = 0; j < merged_solutions[i].size(); j++)
		{
			cout << "[";
			if (merged_solutions[i][j].first == merged_solutions[i][j].second)
			{
				cout << merged_solutions[i][j].first;
			}
			else
			{
				cout << merged_solutions[i][j].first << "," << merged_solutions[i][j].second;
			}
			cout << "]";
			if (j + 1 != merged_solutions[i].size())
			{
				cout << "x";
			}
		}
		cout << ")";
		if (i + 1 != merged_solutions.size())
		{
			cout << "U";
		}
	}
	cout << "\n";
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