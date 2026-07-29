#pragma once

#include <vector>
#include <string>

#include <fstream>

std::vector<std::string> GetFilesRecursive(std::string base_path, std::vector<std::string> whitelist = std::vector<std::string>(), std::vector<std::string> blacklist = std::vector<std::string>());

std::ifstream OpenFile(std::string filename);

int WriteFile(std::string filename, std::vector<std::string> lines, bool add_newlines = true);