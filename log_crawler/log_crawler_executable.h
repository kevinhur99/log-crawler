#ifndef LOG_CRAWLER_EXECUTABLE
#define LOG_CRAWLER_EXECUTABLE

#include "log_crawler_utils.cpp"

using namespace std;
enum class ResSymbol
{
  GREATER,
  LESS,
  EQUAL
};

class LogCrawlerExecutable
{

public:
  map<double, bool> answer;
  // Private Fieldsclear
private:
  string selected_file_tag;
  string selected_file_title;
  string file_path;
  ResSymbol restriction_symbol;
  double restriction_number;

public:
  LogCrawlerExecutable(string file_path, string selected_file_tag, string restriction_symbol, string restriction_number)
  {
    this->file_path = file_path;
    this->selected_file_tag = selected_file_tag;

    if (restriction_symbol == ">")
    {
      this->restriction_symbol = ResSymbol::GREATER;
    }
    else if (restriction_symbol == "<")
    {
      this->restriction_symbol = ResSymbol::LESS;
    }
    else if (restriction_symbol == "=")
    {
      this->restriction_symbol = ResSymbol::EQUAL;
    }
    else
    {
      throw;
    }

    this->restriction_number = stod(restriction_number);
    set_up_selected_file_title();
  }
  ~LogCrawlerExecutable()
  {
  }

  /**
   * Process the data based on the filepath
   * @return {double}  : The total time under restriction
   */
  void process_data();

private:
  void set_up_selected_file_title();
  void evaluate_equation_once();
};

#endif // log_crawler_executable