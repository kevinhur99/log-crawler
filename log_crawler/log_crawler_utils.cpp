#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

class LogCrawlerUtils
{
public:
    /**
 * A function that checks to see if the time is formatted correctly.
 * (2020-01-31_24.59.59)
 * @param  {string} time : The time being evaluated
 * @return {bool}        : Returns true if the time is properly formatted
 */
    bool is_time_formatted_correctly(string time)
    {
        int number_position[] = {0, 1, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18};
        int dash_position[] = {4, 7};
        int dot_position[] = {13, 16};
        int underscore_position[] = {10};

        if (time.length() != 19)
        {
            return false;
        }
        else
        {
            for (auto &i : number_position)
            {
                if (!isdigit(time.at(i)))
                {
                    return false;
                }
            }

            for (auto &i : dash_position)
            {
                if (time.at(i) != '-')
                {
                    return false;
                }
            }

            for (auto &i : dot_position)
            {
                if (time.at(i) != '.')
                {
                    return false;
                }
            }

            if (time.at(10) != '_')
            {
                return false;
            }
        }
        return true;
    }

    /**
 * Check to see if the string is a number
 * @param  {string} s : The string being evaluated
 * @return {bool}     : True if the string is a number
 */
    bool isNumber(string s)
    {
        for (int i = 0; i < s.length(); i++)
        {
            if (!(isdigit(s[i]) || s[i] == '.' || s[i] == '-'))
            {
                return false;
            }
        }
        return true;
    }

    /**
 * Make sure that there are no numbers and that there is a [DAT]
 * @param  {string} line : the line being evaluated
 * @return {bool}        : True if there is no numbers and there is a [DAT]
 */
    bool check_line(string line)
    {
        vector<string> processed_line = process_line(line);
        bool hasDAT = false;
        for (auto word : processed_line)
        {
            // If the char is not any of the numbers, it is false
            if (isNumber(word))
            {
                return false;
            }

            if (word.compare("[DAT]") == 0)
            {
                hasDAT = true;
            }
        }
        return hasDAT;
    }

    /**
 * Convert the lines into a vector of strings with each word
 * @param  {string} line     : The line being evaluated
 * @return {vector<string>}  : The vector of words in each of the line
 */
    vector<string> process_line(string line)
    {
        vector<string> vector_of_words;
        string word = "";
        for (auto x : line)
        {
            if (x == ' ')
            {
                vector_of_words.push_back(word);
                word = "";
            }
            else
            {
                word = word + x;
            }
        }
        vector_of_words.push_back(word);

        return vector_of_words;
    }

    template <typename T>
    /**
 * A function that would find the element in the vector
 * Will return -1 if none were found
 * @param  {vector<T>} vector : The vector being looked at
 * @param  {T} element        : The element that is trying to be found
 * @return {int}              : The index the element is at
 */
    int find_element_in_vector(vector<T> vector, T element)
    {
        int counter = 0;
        for (auto x : vector)
        {
            if (x == element)
            {
                return counter;
            }
            else
            {
                counter++;
            }
        }

        return -1;
    }

    string find_line_based_on_word(string word, string file_path)
    {
        ifstream file;
        string line;
        file.open(file_path, ios::in);
        if (file.is_open())
        {
            while (getline(file, line))
            {
                vector<string> answer_vector = process_line(line);

                for (auto i = answer_vector.begin(); i != answer_vector.end(); ++i)
                {
                    if (word.compare(*i) == 0)
                    {
                        return line;
                    }
                }
            }
        }
        return "";
    }
};