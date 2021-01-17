#include "log_crawler_executable.h"

#ifndef LOG_CRAWLER_EQUATION
#define LOG_CRAWLER_EQUATION

#include <unordered_map>

enum class BoolOperators
{
    AND, //&&
    OR   //||
};

struct EquationData
{
    LogCrawlerExecutable executable;
    int value;
};

class LogCrawlerEquation
{
public:
    double answer;

private:
    string analysis_path_name;
    string equation;
    string file_path;
    vector<pair<int, map<double, bool>>> equation_data;
    vector<pair<int, BoolOperators>> bool_op_data;

public:
    LogCrawlerEquation(string equation, string path, string analysis_path_name)
    {
        this->equation = equation;
        this->file_path = path;
        this->analysis_path_name = analysis_path_name;
        this->equation_data = get_equation_data_from_equation();
        this->bool_op_data = get_bool_op_data_from_equation();
        this->answer = get_answer();
    }

private:
    vector<pair<int, map<double, bool>>> get_equation_data_from_equation();
    vector<pair<int, BoolOperators>> get_bool_op_data_from_equation();
    void evaluate_equation_once();
    double get_answer();
};
#endif // log_crawler_equation
