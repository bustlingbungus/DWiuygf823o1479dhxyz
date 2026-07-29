#include "IO.hpp"
#include "mlsr_glob.hpp"
#include "../Utils.hpp"

#include <iostream>
#include <sstream>

/** used to get user input in `GetUserCommand` */
std::string __input_buffer;


/**
 * uses `std::getline` to read user input command line
 */
void GetUserCommand()
{
    std::cout << cwd << '>';
    // we don't use regular 'cin >>' because it splits input by whitespace in a weird way.
    // it's easier here to just get the whole line of input and parse it ourselves
    std::getline(std::cin, __input_buffer);
}


/**
 * Splits input buffer into seperate arguments, by splitting words at whitespace. Stores all
 * individual arguments into global `cmdarr` array.
 */
void ParseInputBuffer()
{
    cmdarr.clear(); // reset command array

    int l = 0;
    int n = __input_buffer.size();
    for (int i = 0; i < n; ++i)
    {
        char ch = __input_buffer[i];

        // whitespace detected, if length is non-zero, add argument to command array
        if (IsWhitespace(ch))
        {
            int len = i - l;
            if (len > 0)
            {
                // get cmd as a substring and add to array
                std::string cmd = __input_buffer.substr(l, len);
                cmdarr.push_back(cmd);
            }

            l = i + 1; // update length counter
        }
    }

    // after iterating, check if there is a final argument to add
    int len = n - l;
    if (len > 0)
        cmdarr.push_back(__input_buffer.substr(l, len));

}