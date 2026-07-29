#pragma once

#include <string>


int GetParameters(int argc, char** argv, std::string& input_folder, std::string& output_folder);

int parseCommandLine(int argc, char** argv, std::string& input_folder, std::string& output_folder);

std::string CreateOutputLocationWithFilename(std::string outputFolder, std::string filename, std::string newExtension);