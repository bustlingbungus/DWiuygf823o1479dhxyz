#pragma once

#include <string>


int GetParameters(int argc, char** argv, std::string& input_folder, std::string& output_folder, bool& printAny, bool& printVerbose);

int parseCommandLine(int argc, char** argv, std::string& input_folder, std::string& output_folder, bool& printAny, bool& printVerbose);