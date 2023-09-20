/*
Author: Amaan Rahman
Class: ECE6122
Last Date Modified: 09/13/2023
Description:
*/

#ifndef AUXILIARY_H_
#define AUXILIARY_H_

#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// function definitions
bool isNatural(std::string);
int strToNatural(std::string &);

/*
Checks if string is a natural number.

Parameter
----------
`std::string str`: string (user input) to check

Return
------
`bool` on whether the string is a valid natural number or not
*/
bool isNatural(const std::string str)
{
    bool valid = true;
    for (char c : str) // iterate through all characters of `str`
    {
        if (!std::isdigit(c)) // if character is not a digit (0-9), then return `false`
        {
            std::cerr << "[ERROR] Invalid natural number provided!" << std::endl;
            valid = false;
        }
    }
    return valid;
}

bool isFloat(const std::string str)
{
    bool valid = true;
    bool prevIsDigit = false;
    bool hasNegative = false;
    bool hasDecimal = false;
    for (std::size_t i = 0; i < str.size(); ++i)
    {
        char c = str[i];
        if (!(isdigit(c)))
        {
            // check for negative or decimal
            if (c == '-' || c == '.')
            {
                // check if negative sign is first element
                if (!hasNegative && i == 0 && c == '-')
                {
                    hasNegative = true;
                    continue;
                }
                // check if decimal is sandwiched between digits (strict enforcement)
                else if (!hasDecimal && c == '.' && prevIsDigit && (i + 1) < str.size() && isdigit(str[i + 1]))
                {
                    hasDecimal = true;
                    continue;
                }
                // all other permuations of negative or decimal is invalid
                else
                {
                    std::cerr << "[ERROR] Invalid use of negative sign or decimal!" << std::endl;
                    valid = false;
                    break;
                }
            }
            else
            {
                std::cerr << "[ERROR] Invalid character, " << c << " at i=" << i << ", detected. Valid characters are digits, negative sign, and decimal." << std::endl;
                valid = false;
                break;
            }
        }
        prevIsDigit = true;
    }
    return valid;
}

/*
Convert string, given that it is either a valid `double` or `int`.

Parameter
----------
`std::string &str`: reference to user input string to convert to dataype of `value`

Return
------
*/
template <typename T>
bool strToValue(std::string &str, T &value)
{
    bool valid = true;

    // check if string input from user is a valid natural number
    if (std::is_same<T, int>::value && !isNatural(str))
    {
        valid = false;
    }
    else if (std::is_same<T, double>::value && !isFloat(str))
    {
        valid = false;
    }

    // convert string to data type of value
    std::istringstream iss(str);
    iss >> value;

    return valid;
}

#endif