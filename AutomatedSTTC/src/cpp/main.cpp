#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>

#include "utils.hpp"
#include "sys/file_access.hpp"
#include "sys/systemcall.hpp"



int main(int argc, char** argv)
{
    /* ===  PIPELINE PARAMETERS  === */

    std::string input_folder;
    std::string output_folder;

    // get parameters, return any error
    int ret = GetParameters(argc, argv, input_folder, output_folder);
    if (ret != 0) return ret;


    /* ===  SETUP  === */

    // find all png images in input folder
    std::vector<std::string> input_files = GetFilesRecursive(input_folder, {".h5"});


    /* ===  CONVERSION PIPELINE  === */

    // iterate through each detected image 
    int cnt = 0;
    for (auto& recording : input_files)
    {
        ++cnt;
        // if (cnt <= 176) continue;
        std::cout <<'\n'<< recording <<'\n';
        std::cout << "File [ " << cnt << " / " << input_files.size() << " ]\n";

        std::string output_file = CreateOutputLocationWithFilename(output_folder, recording, ".png");

        RunCommands("python", {
            "python", 
            "STTC.py", 
            "\"" + recording + "\"", 
            "\"" + output_file + "\""
        });
    }

    return 0;
}