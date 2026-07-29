/** @todo Test systemcall functionality for macOS + Linux */

#include "systemcall.hpp"

#include <iostream>

#ifdef _WIN32 // windows
#include <windows.h>
#include <direct.h>
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__) // unix OS
#include <stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>
#endif // _WIN32



/**
 * Uses system calls to execute the desired command, and passing an array
 * of arguments onto the command line.
 * 
 * \note For most OS, cmdarr[0] should be the same as cmd. 
 * cmd should be the entire path to the command being executed.
 * 
 * \param cmd The command being executed
 * \param cmdarr Array of arguments being put on the command line
 * 
 * \return `0` for successful execution. 
 * \return `5` if the command failed to execute.
 * \return `4` if this code is compiled using an unsupported operating system
 */
int RunCommands(std::string cmd, std::vector<std::string> cmdarr)
{
#ifdef _WIN32 // windows 

    // convert the argument array into a single command line string
    std::string cmdline = "";
    for (std::string cmd : cmdarr) cmdline += cmd + ' ';

    // set up structure for new process
    PROCESS_INFORMATION processInfo;
    ZeroMemory(&processInfo, sizeof(processInfo));
    STARTUPINFO startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    // create child process using the command line
    if (CreateProcess(
        NULL, 
        (LPSTR)cmdline.c_str(),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &startupInfo,
        &processInfo))
    {
        // wait the the child process to finish executing
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        // close child process when done
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }   
    else // process failed to create
    {
        std::cerr << "Error 005: Failed to execute command \'" << cmdline << "\'\n";
        return 5;
    }

    return 0;

#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__) // unix OS

    // convert cmdarr to null a terminated char** array for execv
    char** c_cmdarr = (char**)malloc((cmdarr.size() + 1) * sizeof(char*));
    if (c_cmdarr == NULL) {
        std::cerr << "Error 013: `malloc` failure.\n";
        return 13;
    }
    // copy command array as c strings
    for (int i = 0; i < cmdarr.size(); ++i) c_cmdarr[i] = cmdarr[i].c_str();
    c_cmdarr[cmdarr.size()] = '\0'; // add null terminator

    // create a new process to call execv in 
    __pid_t p = fork();
    if (p < 0) 
    {
        std::cerr << "Error 005: Failed to execute command \'" << cmd << "\'\n";
        free(c_cmdarr);
        return 5;
    }
    else
    {
        // execute commands in child branch
        if (p == 0)
        {
            execv(cmd.c_str(), c_cmdarr);
            exit(0);
        }
        // wait for child in parent branch
        else
        {
            int status;
            waitpid(p, &status, 0);
        }
    }

    free(c_cmdarr);

#else
    std::cerr << "Error 004: Unsupported operating system\n";
    return 4;
#endif // _WIN32
}


/**
 * Set the current working directory to an absolute or relative directory. 
 * 
 * You may pass '..' as an argument to move to the root directory.
 * 
 * \param dir The directory to move into. May be either absolute or relative
 * 
 * \returns `0` for sucessful execution. Returns a positive integer otherwise.
 */
int setcwd(std::string dir)
{
#ifdef _WIN32 // windows 
    return _chdir(dir.c_str());
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__) // unix OS
    return chdir(dir.c_str());
#else
    std::cerr << "Error 004: Unsupported operating system\n";
    return 4;
#endif // _WIN32
}


/**
 * Returns the current working directory.
 * 
 * You may pass an int pointer to a variable that will return the error code.
 * 
 * Error codes are: 1: Failed to find cwd, 4: Unsupported OS
 * 
 * \param error_code Pointer to a variable to store error code. 
 * Leave as `nullptr` if you don't care.
 */
std::string getcwd()
{

#ifdef _WIN32 // windows
    
    // use char buffer to get cwd
    char* buffer = NULL;
    if ((buffer = _getcwd(NULL, 0)) == NULL) {
        std::cerr << "Error 001: Failed to find cwd.\n";
        return "";
    }
    // convert cwd buffer to string
    return (std::string)buffer;

#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__) // unix OS

    // use char buffer to get cwd
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        std::cerr << "Error 001: Failed to find cwd.\n";
        return "";
    }
    // conver cwd buffer to string
    return (std::string)buffer;

#else
    std::cerr << "Error 004: Unsupported operating system\n";
    return "";
#endif // _WIN32
}