#include "file_access.hpp"

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "systemcall.hpp"



/**
 * Helper to check if a string contains any of the provided substrings
 */
bool str_has_substr(std::string& str, std::vector<std::string>& substrs)
{
    for (std::string& substr : substrs)
    {
        // string contains substring
        if (str.find(substr) != std::string::npos)
            return true;
    }
    return false; // no substring found
}


/**
 * Helper function to recursively search for files in a directory.
 */
void GetFilesRecursive_Help(std::string& path, std::vector<std::string>& res, std::vector<std::string>& whitelist, std::vector<std::string>& blacklist)
{
    // iterate over files in the current path
    for (const auto& entry : fs::directory_iterator(path))
    {
        // get entry as a string
        std::string path_str = entry.path().string();


        // If any blacklisted substring is found quit now, since 
        // all following paths from this one will inherently contain the blakclisted string  
        if (str_has_substr(path_str, blacklist)) continue;


        // path is directory, search sub directories
        if (fs::is_directory(entry.status()))
            GetFilesRecursive_Help(path_str, res, whitelist, blacklist);

        // path is file, not directory
        else
        {
            // search whitelist to make sure final file contains a whitelisted substring
            // if whitelist is empty don't worry lol
            if (!whitelist.empty())
                if (!str_has_substr(path_str, whitelist)) 
                    continue;

            // final file contains whitelisted string with no blacklisted strings, add to result
            res.push_back(path_str);
        }
    }
}


/**
 * Recursively search directories to find files. Search is started within the `base_path` directory.
 * Output array will contain paths to valid files found within the directory. It will ONLY
 * contain files, no folders. 
 * 
 * \param base_path The base directory to begin the recursive search from.
 * \param whitelist At least ONE of the substrings in this array must appear in a file's path to be included.
 * If left empty, no check is performed.
 * \param blacklist If ANY of the substrings in this array are found in a file's path, it will not be included.
 */
std::vector<std::string> GetFilesRecursive(std::string base_path, std::vector<std::string> whitelist, std::vector<std::string> blacklist)
{
    std::vector<std::string> res;
    GetFilesRecursive_Help(base_path, res, whitelist, blacklist);
    return res;
}


/**
 * Returns an input file stream of the filename specified. If the filename is invalid, 
 * output will evaluate `false` when typecasted to bool.
 */
std::ifstream OpenFile(std::string filename)
{
    return std::ifstream(filename);
}


/**
 * Writes information to the specified filename.
 * Each line is written to the file one by one. if `add_newlines` is `true`, a 
 * newline character will automatically be added to the end of each string in `lines`.
 * 
 * \note If the specified filename already exists, the file will be overwritten.
 * 
 * \param filename The file to output into
 * \param lines Array of individual lines to write to file
 * \param add_newlines True to add a newline character to each line\
 * 
 * \return 
 */
int WriteFile(std::string filename, std::vector<std::string> lines, bool add_newlines)
{
    // open file in output mode
    std::ofstream o(filename);

    // failed to open file to write
    if (!o.is_open()) {
        std::cerr << "Error 012: Failed to open \'"<<filename<<"\' in output mode.\n";
        return 12;
    }

    // write each line to file
    for (std::string& l : lines)
    {
        o << l;   
        if (add_newlines) o << '\n'; // newline requested 
    }

    // close file and return success
    o.close();
    return 0;
}


/**
 * Creates the specified directory, either absolute or relative.
 * 
 * \warning If it is not possible to create the provided directory (eg. invalid folder name), this function may crash, or change cwd
 * 
 * \param directory_name Name of the directory to create
 */
void CreateDirectory(std::string directory_name)
{
    // get original cwd
    std::string owd = getcwd();

    // iterate through string, whenever a slash is encountered, create a directory.
    // since you can only create directories within the cwd, we will change to the create folder
    // at every iteration, to iteratively create all folders in the given path.
    int last_dir_idx = 0;
    for (int i = 0; i <= directory_name.size(); ++i)
    {
        // slash or end of file, create new directory
        if (i == directory_name.size() || directory_name[i] == '/' || directory_name[i] == '\\')
        {
            // get folder name of new directory
            std::string dir = directory_name.substr(last_dir_idx, i - last_dir_idx);
            
            if (dir.size() > 0) // avoid empty folder names
            {
                // colon represents absolute system directory; add slash so we can enter it properly
                if (dir[dir.size()-1] == ':') dir += '/';
                // '..' means we will change up a directory, dont add a '..' folder
                // otherwise, create the folder
                else if (dir != "..") fs::create_directory(dir);
            }
            // update index and enter the new folder
            last_dir_idx = i + 1;
            setcwd(dir);
        }
    }

    // restore cwd
    setcwd(owd);
}