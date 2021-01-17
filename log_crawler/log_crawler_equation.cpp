#include "log_crawler_equation.h"

extern LogCrawlerUtils utils;

vector<pair<int, map<double, bool>>> LogCrawlerEquation::get_equation_data_from_equation()
{
    vector<pair<int, map<double, bool>>> answer;
    vector<string> processed_line = utils.process_line(this->equation);

    int last_value = 0;
    for (int i = 0; i < processed_line.size(); i++)
    {
        string equation = processed_line.at(i);
        int increase = 0;
        int decrease = 0;

        if (equation.find("/") != string::npos)
        {
            increase = count(equation.begin(), equation.end(), '(');
            equation.erase(remove(equation.begin(), equation.end(), '('), equation.end());

            string file_path = this->file_path + "\\log\\" + equation.substr(1, equation.find('/') - 1);
            string tag_name = equation.substr(equation.find('/') + 1, equation.size());

            equation = processed_line.at(i + 1);
            string res_symbol = equation;

            equation = processed_line.at(i + 2);
            decrease = count(equation.begin(), equation.end(), ')');
            equation.erase(remove(equation.begin(), equation.end(), ')'), equation.end());
            string res_number = equation;

            last_value += increase;

            LogCrawlerExecutable temp(file_path, tag_name, res_symbol, res_number);
            temp.process_data();

            if (temp.answer.empty())
            {
                ofstream output_file;
                output_file.open(this->analysis_path_name, ios_base::app);
                output_file << "There are no values for " << tag_name << endl;
                cout << "\rThere are no values for " << tag_name << endl;
                output_file.close();
            }
            answer.push_back(pair<int, map<double, bool>>(last_value, temp.answer));
        }
        last_value -= decrease;
    }
    return answer;
}

vector<pair<int, BoolOperators>> LogCrawlerEquation::get_bool_op_data_from_equation()
{
    vector<pair<int, BoolOperators>> answer;
    vector<string> processed_line = utils.process_line(this->equation);

    int last_value = 0;
    for (int i = 0; i < processed_line.size(); i++)
    {
        string equation = processed_line.at(i);
        int increase = count(equation.begin(), equation.end(), '(');
        int decrease = count(equation.begin(), equation.end(), ')');
        last_value += increase;

        if (equation == "&&" || equation == "||")
        {
            BoolOperators bool_op;
            if (equation == "&&")
            {
                bool_op = BoolOperators::AND;
            }
            else
            {
                bool_op = BoolOperators::OR;
            }

            answer.push_back(pair<int, BoolOperators>(last_value, bool_op));
        }
        last_value -= decrease;
    }

    return answer;
}

map<double, bool> merge_helper(map<double, bool> first_map, map<double, bool> second_map, bool is_and)
{
    auto it1 = first_map.begin();
    auto it2 = second_map.begin();

    for (; it1 != first_map.end() && it2 != second_map.end();)
    {
        if (it1->first < it2->first)
        {
            ++it1;
        }
        else if (it1->first > it2->first)
        {
            if (it2->second == is_and)
            {
                if (it1 == first_map.begin())
                {
                    first_map.insert({it2->first, !is_and});
                }
                else
                {
                    --it1;
                    bool temp = it1->second;
                    first_map.insert({it2->first, temp});
                    ++it1;
                }
            }
            else
            {
                first_map.insert(it1, *it2);
            }
            ++it2;
        }
        else
        { // equivalent
            if (is_and)
            {
                it1->second = it1->second && it2->second;
            }
            else
            {
                it1->second = it1->second || it2->second;
            }
            ++it1;
            ++it2;
        }
    }

    first_map.insert(it2, second_map.end());
    return first_map;
}

map<double, bool> merge(map<double, bool> first_map, map<double, bool> second_map, bool is_and)
{
    map<double, bool> merged_map_one = merge_helper(first_map, second_map, is_and);
    map<double, bool> merged_map_two = merge_helper(second_map, first_map, is_and);

    auto it1 = merged_map_one.begin();
    auto it2 = merged_map_two.begin();
    auto &&comp = merged_map_one.value_comp();

    for (; it1 != merged_map_one.end() && it2 != merged_map_two.end();)
    {
        if (comp(*it1, *it2))
        {
            ++it1;
        }
        else if (comp(*it2, *it1))
        {
            merged_map_one.insert(it1, *it2); // with hint to have correct complexity
            ++it2;
        }
        else
        { // equivalent

            if (is_and)
            {
                it1->second = it1->second && it2->second;
            }
            else
            {
                it1->second = it1->second || it2->second;
            }
            ++it1;
            ++it2;
        }
    }
    merged_map_one.insert(it2, merged_map_two.end());

    return merged_map_one;
}

template <class Answer>
vector<pair<int, Answer>> find_highest_key_in_map(vector<pair<int, Answer>> eval)
{
    int max = 0;
    vector<pair<int, Answer>> answer;
    int counter = 0;

    for (pair<int, Answer> pair : eval)
    {
        if (pair.first > max)
        {
            answer.clear();
            answer.push_back(pair);
            max = pair.first;
        }
        else if (pair.first == max)
        {
            answer.push_back(pair);
        }
    }
    return answer;
}

template <class Answer>
void update(vector<pair<int, Answer>> *original, vector<pair<int, Answer>> blacklist_pair)
{

    int last_value = 0;
    for (int j = 0; j < blacklist_pair.size(); j++)
    {
        for (int i = 0; i < original->size(); i++)
        {
            if (blacklist_pair.at(j).first == original->at(i).first)
            {
                original->erase(original->begin() + i);
                last_value = blacklist_pair.at(j).first;
            }
        }
    }
}

void LogCrawlerEquation::evaluate_equation_once()
{
    map<double, bool> answer;
    vector<pair<int, map<double, bool>>> equation_pair = find_highest_key_in_map(this->equation_data);
    vector<pair<int, BoolOperators>> bool_op = find_highest_key_in_map(this->bool_op_data);

    if (bool_op.at(0).second == BoolOperators::AND)
    {
        answer = merge(equation_pair.at(0).second, equation_pair.at(1).second, true);
    }
    else
    {
        answer = merge(equation_pair.at(0).second, equation_pair.at(1).second, false);
    }
    update(&this->equation_data, equation_pair);
    this->equation_data.push_back(pair<int, map<double, bool>>(equation_pair.at(0).first - 1, answer));
    update(&this->bool_op_data, bool_op);
}

double LogCrawlerEquation::get_answer()
{
    while (this->equation_data.size() != 1)
    {
        evaluate_equation_once();
    }

    map<double, bool> final_map = equation_data.at(0).second;

    double total_time_under_restriction = 0;
    bool was_previous_restricted = true;
    bool is_current_restricted = true;
    double newBeginning = 0;
    double newEnd = 0;
    double time = 0;
    map<double, bool>::iterator itr;
    for (itr = final_map.begin(); itr != final_map.end(); ++itr)
    {
        is_current_restricted = !itr->second;
        time = itr->first;
        if (was_previous_restricted && !is_current_restricted)
        {
            newBeginning = time;
        }
        else if (!was_previous_restricted && is_current_restricted)
        {
            newEnd = time;
            total_time_under_restriction += (newEnd - newBeginning);
        }
        was_previous_restricted = is_current_restricted;
    }

    if (!was_previous_restricted && !is_current_restricted)
    {
        newEnd = time;
        total_time_under_restriction += (newEnd - newBeginning);
    }

    ofstream output_file;
    output_file.open(this->analysis_path_name, ios_base::app);
    output_file << "Total time under restriction: " << total_time_under_restriction << endl
                << endl;
    cout << "\rTotal time under restriction: " << total_time_under_restriction << endl
         << endl;
    output_file.close();
    return total_time_under_restriction;
}
