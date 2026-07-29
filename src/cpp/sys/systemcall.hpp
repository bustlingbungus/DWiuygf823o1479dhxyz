#pragma once

#include <vector>
#include <string>

int RunCommands(std::string cmd, std::vector<std::string> cmdarr);

int setcwd(std::string dir);

std::string getcwd();