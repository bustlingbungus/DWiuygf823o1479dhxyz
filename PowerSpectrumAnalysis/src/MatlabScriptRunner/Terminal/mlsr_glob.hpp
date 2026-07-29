#pragma once

#include <string>
#include <vector>
#include <memory>

class ScriptConfig; // forwards declaration

extern std::string cwd;
extern std::vector<std::string> cmdarr;
extern std::shared_ptr<ScriptConfig> scriptConfig;

int __mlsr_init();

void __mlsr_close(int exit_code);
