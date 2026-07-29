#include "utils.hpp"

#include <iostream>

#include "sys/file_access.hpp"


/**
 * Gets parameters for the program. If there are command line arguments (argc > 1), parameters will be read from
 * the command line. Otherwise, they are hard coded in this function
 */
int GetParameters(int argc, char** argv, std::string& input_folder, std::string& output_folder)
{
    if (argc > 1) // get parameters from command line
    {
        return parseCommandLine(argc, argv, input_folder, output_folder);
    }
    else // params from user input
    {
        std::cout << "Input root directory: "; std::getline(std::cin, input_folder);
        std::cout << "Output root directory: "; std::getline(std::cin, output_folder);
        // fix output directory
        int n = output_folder.size();
        if (n > 0)
        {
            if (output_folder[n-1] != '/' && output_folder[n-1] != '\\')
                output_folder += '\\';
        }
    }
    return 0;
}


/**
 * Parses the command line for flags and arguments to set input/output locations,
 * as well as print settings.
 */
int parseCommandLine(int argc, char** argv, std::string& input_folder, std::string& output_folder)
{
    input_folder = argv[1];
    output_folder = argv[2];

    // error handle i/o folders
    if (input_folder == "") {
        std::cerr << "Missing input folder. Aborting program\n";
        return 1;
    }
    if (output_folder == "") {
        std::cerr << "Missing output folder. Aborting program\n";
        return 2;
    }

    return 0; // return success
}


/**
 * Creates a directory in the output folder for the file's animal, plate, and div. Returns a filename in 
 * the new output directory, with the same name as `filename` paramter, but using the provided extension
 * 
 * \param outputFolder The root directory to store output in
 * \param filename The absolute directory of the file, containing actual filename, and extension
 * \param newExtension The file extension the returned string should have 
 */
std::string CreateOutputLocationWithFilename(std::string outputFolder, std::string filename, std::string newExtension)
{
    // remove original extension
    int idx = filename.find(".h5");
    // replace with desired file extension
    std::string res = filename.substr(0, idx) + newExtension;
    
    // find the last slash in the filename (separates the actual FILE name from the directory name)
    for (int i = idx - 1; i >= 0; --i)
    {
        // set result equal to the FILE name, and save index
        if (filename[i] == '\\' || filename[i] == '/')
        {
            idx = i;
            res = res.substr(i + 1, res.size() - i);
            break;
        }
    }

    // continuing from the index of the last slash, find 'N', which indicates the animal folder
    for (int i = idx; i >= 0; --i)
    {
        // N found
        if (filename[i] == 'N')
        {
            // create a new directory of the animal, plate, and div folders, but within the output folder
            std::string folder = filename.substr(i, idx - i + 1);
            CreateDirectory(outputFolder + "\\" + folder);
            // resulting filename is the new output folder, plus the actual FILE name
            res = folder + res;
            break;
        }
    }

    // add the root output folder to the file name and return
    res = outputFolder + "\\" + res;
    return res;
}