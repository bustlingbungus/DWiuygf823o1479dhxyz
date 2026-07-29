#include "commands.hpp"

#include <iostream>


/**
 * Gets parameters for the program. If there are command line arguments (argc > 1), parameters will be read from
 * the command line. Otherwise, they are hard coded in this function
 */
int GetParameters(int argc, char** argv, std::string& input_folder, std::string& output_folder, bool& printAny, bool& printVerbose)
{
    if (argc > 1) // get parameters from command line
    {
        return parseCommandLine(argc, argv, input_folder, output_folder, printAny, printVerbose);
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

        printAny = true;
        printVerbose = true;
    }
    return 0;
}


/**
 * Parses the command line for flags and arguments to set input/output locations,
 * as well as print settings.
 */
int parseCommandLine(int argc, char** argv, std::string& input_folder, std::string& output_folder, bool& printAny, bool& printVerbose)
{
    // initialize parmeters
    input_folder = "";
    output_folder = "";
    printAny = true;
    printVerbose = false;

    // iterate through arguments (arg 0 is the exe name)
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        // input flag saying next argument is input folder
        // assign input to next argument, and skip next argument
        if ((arg == "-input" || arg == "-i" || arg == "-in") && i < argc-1) input_folder = argv[++i];
        // output flag saying next argument is output folder
        // assign output to next argument, and skip next argument
        else if ((arg == "-output" || arg == "-o" || arg == "-out") && i < argc-1) output_folder = argv[++i];
        // batch flag means don't print to console
        else if (arg == "-batch" || arg == "-b") printAny = false;
        // set verbose printing true
        else if (arg == "-verbose" || arg == "-v") printVerbose = true;
        else
        {
            std::cerr << "Unreconized argument \'" << arg << "\'. Aborting program\n";
            return -1;
        }
    }

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