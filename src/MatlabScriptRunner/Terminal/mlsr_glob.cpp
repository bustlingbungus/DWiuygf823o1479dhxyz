#include "mlsr_glob.hpp"
#include "../System/systemcall.hpp"
#include "../ScriptRun/ScriptConfig.hpp"

#include <iostream>
#include <direct.h>


/** Current working directory */
std::string cwd;

/** Array of commands input by user on command line */
std::vector<std::string> cmdarr;

/** Global script config for saving/loading/storing parameters. */
std::shared_ptr<ScriptConfig> scriptConfig = nullptr;

/** true if application has been initialized. */
bool is_init = false;



/**
 * Initializes the script runner program. Call once before doing anything.
 * 
 * \returns 0 for success. Positive value otherwise.
 */
int __mlsr_init()
{
    // don't do all this when program is already initialized
    if (!is_init)
    {
        // get current working directory
        cwd = getcwd();
        if (cwd == "") return 1;
    
        // create script config object
        scriptConfig = std::make_shared<ScriptConfig>();
        if (scriptConfig == nullptr) {
            std::cerr << "Error 008: Failed to initialize config\n";
            return 8;
        }
        // load parameters from file
        scriptConfig->Load();
    
        // initialization complete
        is_init = true;
    }

    return 0;
}


/**
 * Saves program configurations to file and exits the program. Program will
 * exit with the provided exit code.
 */
void __mlsr_close(int exit_code)
{
    scriptConfig->Save();
    exit(exit_code);
}