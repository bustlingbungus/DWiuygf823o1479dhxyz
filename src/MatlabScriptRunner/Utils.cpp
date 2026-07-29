#include "Utils.hpp"



/**
 * Returns true if the input character is any type of whitespace character
 */
bool IsWhitespace(const char& ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\f' || ch == '\r';
}


/**
 * Returns `true` if the input string contains any non-whitespace characters.
 * Returns `false` otherwise.
 */
bool HasNonWhitespace(const std::string& str)
{
    for (const char& ch : str)
    {
        if (!IsWhitespace(ch)) return true;
    }

    return false;
}


/**
 * Formats filenames, does not check if the provided filename is valid and can be used to load
 * files.
 * 
 * Removes leading and trailing whitespace and quotes from the filename. Makes all slashes
 * in the path be the preferred slash (forwards by default). Will make sure the filename doesn't
 * end with a slash, unless `endWithSlash` is true, then the filename will be made to end with
 * a slash.
 * 
 * \param filename The raw filename string to format
 * \param slashDirection the preferred type of slash to use for paths in the filename (forwards by default)
 * \param endsWithSlash `true` to ensure the filename ends with a slash, `false` to ensure it does not (false by default)
 */
std::string FormatFilename(std::string filename, const SlashDirection& slashDirection, const bool& endWithSlash)
{
    // find first and last non-whitespace non-quote characters
    auto l = filename.begin();
    auto r = filename.end();
    for (; l < r; ++l) {
        if (!IsWhitespace(*l) || *l != '\'' || *l != '\"') break;
    }
    for (--r; r >= l; --r) {
        if (!IsWhitespace(*r) || *r != '\'' || *r != '\"') break;
    }

    // trim the filename to not include leading/trailing whitespace/quotes
    filename = filename.substr(l - filename.begin(), r - l + 1);

    if (!filename.empty())
    {
        // replace all slashes in the filename with the preferred slash direction
        char bad_slash = slashDirection == SLASHDIR_Forwards ? '\\' : '/';
        char good_slash = slashDirection == SLASHDIR_Forwards ? '/' : '\\';
        for (auto it = filename.begin(); it < filename.end(); ++it)
        {
            if (*it == bad_slash) *it = good_slash;
        }
    
        // if the filename does not end with a slash when it should, add a slash
        // if the filename ends with a slash when it shouldn't, remove it
        if (*(filename.end() - 1) != good_slash) {
            if (endWithSlash) filename += good_slash;
        } else if (!endWithSlash) filename.pop_back();
    }

    return filename; // return formatted filename
}


/**
 * Returns true if the input string only contains Y or y. Returns false if the input
 * string is empty/whitespace, or contains any character other then y, Y and whitespace.
 */
bool YNresponse(const std::string& str)
{
    // initially false, since empty response = false
    bool res = false;
    for (const char& ch : str)
    {
        if (!IsWhitespace(ch))
        {
            // if non-y character is found, response is a no. 
            // if response only contains Y's, it is yes.
            if (ch != 'Y' && ch != 'y') return false;
            res = true;
        }
    }
    return res;
}