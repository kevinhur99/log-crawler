#include "log_crawler_equation.h"

extern LogCrawlerUtils utils;

vector<string> get_folder_contents(string pathname_string)
{
  // Get the restriction folder file path
  struct stat info;
  char pathname[pathname_string.length()];
  strcpy(pathname, pathname_string.c_str());

  // Open the directory and get a list all of the restriction foler contents
  DIR *dir;
  struct dirent *entry;
  vector<string> file_paths;
  dir = opendir(pathname);
  while ((entry = readdir(dir)) != NULL)
  {
    if (!(entry->d_name[0] == '.'))
    {
      string full_path = pathname_string + "\\" + entry->d_name;

      file_paths.push_back(full_path);
    }
  }
  closedir(dir);

  if (file_paths.size() == 0)
  {
    cout << "Could not find restriction_folder/analysis_folder in same location as this executable." << endl;
    cout << pathname << endl;
  }

  // Return the the vector of strings of teh file contents
  sort(file_paths.begin(), file_paths.end());
  return file_paths;
}

string get_file_path_time(string file_path)
{
  string file_path_time;
  string answer;

  for (int i = 0; i < file_path.length(); i++)
  {
    answer = file_path.substr(i, 19);
    if (utils.is_time_formatted_correctly(answer))
    {
      break;
    }
  }

  return answer;
}

string *get_file_paths_time(vector<string> file_paths)
{
  string *file_paths_time = new string[file_paths.size()];

  for (int i = 0; i < file_paths.size(); i++)
  {
    file_paths_time[i] = get_file_path_time(file_paths.at(i));
  }

  return file_paths_time;
}

void update_list_of_paths(vector<string> &file_paths, string start_time, string end_time)
{
  // Convert the start and end time into the actual times that are available.
  int start_position;
  int end_position;
  string *file_paths_time;
  file_paths_time = get_file_paths_time(file_paths);

  if (start_time.compare("start") == 0)
  {
    start_position = 0;
  }
  else
  {
    for (int i = 0; i < file_paths.size(); i++)
    {
      if (start_time.compare(file_paths_time[i]) <= 0)
      {
        start_position = i;
        break;
      }
    }
  }

  if (end_time.compare("end") == 0)
  {
    end_position = file_paths.size() - 1;
  }
  else
  {
    for (int i = file_paths.size() - 1; i > 0; i--)
    {
      if (end_time.compare(file_paths_time[i]) >= 0)
      {
        end_position = i;
        break;
      }
    }
  }

  // Have the file path field only include the paths that are within the start
  // and end time
  vector<string> new_file_path;
  for (int i = start_position; i < end_position + 1; i++)
  {
    new_file_path.push_back(file_paths.at(i));
  }
  file_paths = new_file_path;
}

string create_analysis_folder(string file_path, string file_name)
{

  string analysis_file_name = "analysis_" + file_name;
  string analysis_full_file_path = file_path + "\\" + analysis_file_name;

  vector<string> list_of_paths = get_folder_contents(file_path);
  int counter = 1;
  while (utils.find_element_in_vector(list_of_paths, analysis_full_file_path) != -1)
  {
    analysis_file_name = analysis_file_name.substr(0, analysis_file_name.find_first_of("."));
    analysis_file_name = analysis_file_name.substr(0, analysis_file_name.find_first_of("("));
    analysis_file_name = analysis_file_name + "(" + to_string(counter) + ")" + ".txt";
    analysis_full_file_path = file_path + "\\" + analysis_file_name;
    counter++;
  }

  cout << "Putting data at location: " << analysis_full_file_path << endl;
  return analysis_full_file_path;
}

bool path_has_sim(string file_path, string out_file)
{
  vector<string> folder_contents = get_folder_contents(file_path);
  ofstream output_file;
  output_file.open(out_file, ios_base::app);

  for (string path : folder_contents)
  {
    string log_file_name = path.substr(path.find_last_of("\\", path.size()));
    if (log_file_name.find("_sim_") != -1)
    {
      cout << "\rSkipping directory... Suspected to be simulated\n"
           << endl;
      output_file << "Skipping directory... Suspected to be simulated\n"
                  << endl;
      output_file.close();
      return true;
    }

    if (log_file_name.find("simulation_manager_node.log") != -1)
    {
      cout << "\rSkipping directory... Suspected to be simulated\n"
           << endl;
      output_file << "Skipping directory... Suspected to be simulated\n"
                  << endl;
      output_file.close();
      return true;
    }
  }
  output_file.close();
  return false;
}

void restriction_folder_protocol(string file_path)
{
  ifstream file;
  string line;
  file.open(file_path, ios::in);

  string base_path, start_time, end_time, equation;
  bool include_sim;
  vector<string> list_of_paths;

  if (file.is_open())
  {
    getline(file, line);
    base_path = line;
    list_of_paths = get_folder_contents(base_path);

    getline(file, line);
    start_time = line;
    getline(file, line);
    end_time = line;
    update_list_of_paths(list_of_paths, start_time, end_time);

    getline(file, line);
    equation = line;
    getline(file, line);
    string temp_include_sim = line;

    if (temp_include_sim == "0")
    {
      include_sim = false;
    }
    else
    {
      include_sim = true;
    }

    file.close();
  }

  string analysis_file_path = file_path.substr(0, file_path.find("\\restriction_folder")) + "\\analysis_folder";
  string analysis_file_name = file_path.substr(file_path.find_last_of("\\") + 1, file_path.length());
  analysis_file_path = create_analysis_folder(analysis_file_path, analysis_file_name);

  ofstream output_file;
  output_file.open(analysis_file_path);

  output_file << "---------------------------------------------------------------------------" << endl;
  output_file << "ANALYZING: " << file_path << endl;
  output_file << "BASE PATH: " << base_path << endl;
  output_file << "START TIME: " << start_time << endl;
  output_file << "END TIME: " << end_time << endl;
  output_file << "EQUATION: " << equation << endl;
  output_file << "INCLUDE SIMULATION?: " << include_sim << endl;
  output_file << "---------------------------------------------------------------------------" << endl
              << endl;

  cout << "---------------------------------------------------------------------------" << endl;
  cout << "ANALYZING: " << file_path << endl;
  cout << "BASE PATH: " << base_path << endl;
  cout << "START TIME: " << start_time << endl;
  cout << "END TIME: " << end_time << endl;
  cout << "EQUATION: " << equation << endl;
  cout << "INCLUDE SIMULATION?: " << include_sim << endl;
  cout << "---------------------------------------------------------------------------" << endl
       << endl;
  output_file.close();

  int counter = 0;
  double total = 0;
  for (string path : list_of_paths)
  {
    counter++;
    ofstream output_file;
    output_file.open(analysis_file_path, ios_base::app);
    cout << path << endl;
    output_file << path << endl;
    output_file.close();

    cout << "\rProgress... " << counter << "/" << list_of_paths.size() << " Files";

    if (include_sim || !path_has_sim(path + "\\log", analysis_file_path))
    {
      LogCrawlerEquation log_crawler_equation = LogCrawlerEquation(equation, path, analysis_file_path);
      total += log_crawler_equation.answer;
    }
  }

  output_file.open(analysis_file_path, ios_base::app);
  output_file << "\nThe total time for all files under restriction: " << total << endl;
  cout << "\nThe total time for all files under restriction: " << total << endl
       << endl;

  output_file.close();
}

double user_input_protocol()
{
  return 0;
}

int main(int argc, char *argv[])
{
  string home_path(argv[0]);
  home_path = home_path.substr(0, home_path.find_last_of('\\'));
  string pathname_string = home_path + "\\restriction_folder";
  vector<string> restriction_folder_contents = get_folder_contents(pathname_string);

  if (restriction_folder_contents.size() > 0)
  {
    for (string file_path : restriction_folder_contents)
    {
      restriction_folder_protocol(file_path);
    }
  }
  else
  {
    user_input_protocol();
  }

  cout << "\nPress ENTER to exit\n";
  cin.ignore();
  cin.get();
  return 0;
}
