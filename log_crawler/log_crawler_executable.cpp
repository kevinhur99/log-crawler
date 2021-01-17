#include "log_crawler_executable.h"

LogCrawlerUtils utils;

/**
 * A function that will check to see if a line has title and data.
 * Meaning, that it checks to see if it has the given title and numbers.
 * anywhere.
 * @param  {string} line  : The line being looked at
 * @param  {string} title : See if the line has a title
 * @return {bool}         : True if it has title and data. False otherwise
 */
bool line_has_title_and_data(string line, string title)
{
  // Has the substring for the tag and contains numbers (Assumed to be data)

  vector<string> processed_line = utils.process_line(line);
  bool hasNumber = false;
  for (auto word : processed_line)
  {
    // If the char is not any of the numbers, it is false
    if (utils.isNumber(word))
    {
      hasNumber = true;
    }
  }
  return (line.find(title) != string::npos) && hasNumber;
}

// AKA get rid of all char before the last instance of ]
/**
 * Get rid of the time and title
 * @param  {string} line : The line being evaluated
 * @return {string}      : The string after the time and title has been removed
 */
string get_rid_of_time_and_title(string line)
{
  string temp = line;
  int pos_of_last_close_bracket = temp.find_last_of("]");
  string answer = temp.erase(0, pos_of_last_close_bracket);
  return answer;
}

int get_selected_tag_number(string file_path, string selected_file_tag)
{
  int counter = 0;

  string line = utils.find_line_based_on_word(selected_file_tag, file_path);
  string no_time_and_title_line = get_rid_of_time_and_title(line);
  vector<string> answer_vector = utils.process_line(no_time_and_title_line);
  for (auto i = answer_vector.begin(); i != answer_vector.end(); ++i)
  {
    if (selected_file_tag.compare(*i) == 0)
    {
      return counter;
    }
    else
    {
      counter++;
    }
  }
}

// Under the assumption that the line has the appropriate title.
/**
 * Get the data that should be evaluated based on the selected tags
 * @param  {string} line             : The line that is being evaluated
 * @param  {int} selected_tag_number : The selected tag number
 * @return {double}                  : The data that is going to be evaluated
 */
double get_data_to_be_restricted(string line, int selected_tag_number)
{
  string new_line = get_rid_of_time_and_title(line);
  vector<string> new_line_vector = utils.process_line(new_line);
  return stod(new_line_vector.at(selected_tag_number));
}

/**
 * Check to see if the data is restricted
 * @param  {double} restriction_number    : The number that is restricted
 * @param  {ResSymbol} restriction_symbol : The sumbol for the restriction
 * @param  {double} data                  : The data being evaluated
 * @return {bool}                         : True if the data is not meeting the
 * conditions of the restrictions
 */
bool isRestricted(double restriction_number, ResSymbol restriction_symbol, double data)
{
  switch (restriction_symbol)
  {
  case ResSymbol::GREATER:
    return !(data > restriction_number);

  case ResSymbol::LESS:
    return !(data < restriction_number);

  case ResSymbol::EQUAL:
    return !(data == restriction_number);

  default:
    throw;
  }
}

/**
 * Get the time from the line
 * @param  {string} line : The line being evaluated
 * @return {double}      : the time from the line
 */
double get_time(string line)
{
  return stod(line.substr(1, line.find_first_of("]")));
}

void LogCrawlerExecutable::set_up_selected_file_title()
{
  ifstream file;
  string line;
  file.open(this->file_path, ios::in);

  if (file.is_open())
  {
    while (getline(file, line))
    {
      string only_tags = line.substr(line.find_last_of("]") + 2, line.length());
      vector<string> only_tags_vector = utils.process_line(only_tags);

      if (utils.find_element_in_vector(only_tags_vector, this->selected_file_tag) != -1)
      {
        int last_pos_of_time = line.find_first_of("]");

        int last_pos_of_title = line.find_last_of("]");
        file.close();
        this->selected_file_title = line.substr(
            last_pos_of_time, last_pos_of_title - last_pos_of_time + 1);
        break;
      }
    }
  }
}

void LogCrawlerExecutable::process_data()
{
  ifstream file;
  string line;
  file.open(this->file_path, ios::in);
  try
  {
    if (file.is_open())
    {
      while (getline(file, line))
      {
        if (line_has_title_and_data(line, this->selected_file_title))
        {
          // Get the data that is to be evalulated
          int selected_tag_number = get_selected_tag_number(this->file_path, this->selected_file_tag);
          double data = get_data_to_be_restricted(line, selected_tag_number);
          double time = get_time(line);
          bool is_current_restricted = isRestricted(this->restriction_number, this->restriction_symbol, data);

          if (!is_current_restricted)
          {
            this->answer.insert(pair<double, bool>(time, true));
          }
          else
          {
            this->answer.insert(pair<double, bool>(time, false));
          }
        }
      }
      file.close();
    }
  }
  catch (exception e)
  {
  }
}
