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
    std::cerr << "Unsupported operating system\n";
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
        std::cerr << "Failed to find cwd.\n";
        return "";
    }
    // convert cwd buffer to string
    return (std::string)buffer;

#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__) // unix OS

    // use char buffer to get cwd
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        std::cerr << "Failed to find cwd.\n";
        return "";
    }
    // conver cwd buffer to string
    return (std::string)buffer;

#else
    std::cerr << "Unsupported operating system\n";
    return "";
#endif // _WIN32
}