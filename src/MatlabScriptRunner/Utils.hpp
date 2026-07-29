#pragma once

#include <string>

/** Define whether rowards or backwards slashes should be used for file names */
enum SlashDirection
{
    SLASHDIR_Forwards,
    SLASHDIR_Backwards
};


bool IsWhitespace(const char& ch);

bool HasNonWhitespace(const std::string& str);

std::string FormatFilename(std::string filename, const SlashDirection& slashDirection = SLASHDIR_Forwards, const bool& endWithSlash = false);

bool YNresponse(const std::string& str);