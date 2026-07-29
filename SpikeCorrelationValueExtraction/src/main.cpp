#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>

#include "sdl/LSurface.hpp"
#include "utils.hpp"
#include "ColorToNumber.hpp"
#include "sys/file_access.hpp"
#include "commands.hpp"
#include "Spreadsheet.hpp"



int main(int argc, char** argv)
{
    /* ===  PIPELINE PARAMETERS  === */

    std::string input_folder;
    std::string output_folder;
    bool printAny;
    bool printVerbose;

    // get parameters, return any error
    int ret = GetParameters(argc, argv, input_folder, output_folder, printAny, printVerbose);
    if (ret != 0) return ret;


    /* ===  SETUP  === */

    // create spreadsheet object to store values, and LSurface to load images
    Spreadsheet spreadsheet;
    std::shared_ptr<LSurface> spikeImage = std::make_shared<LSurface>();

    // find all png images in input folder
    std::vector<std::string> input_files = GetFilesRecursive(input_folder, {".png"});    

    // get legend, legend data should be stored alongside the executable
    std::unordered_map<int,float> legend_map = GetLegend("legend_map.txt");


    /* ===  CONVERSION PIPELINE  === */

    // iterate through each detected image 
    for (auto& img : input_files)
    {
        /**
         * This is the code that will detect cell locations in the figure, and generate 0 - 1 
         * values for each cell. These values are saved in a 2D array, where entry [0][0] is the 
         * top cell, [1][0] is the first cell on the second row, and so on. 
         * 
         * This code will also find the total of all cell values, and the average cell value, and
         * will save this, along with all raw cell values, to a file in the output directory.
         */

        // load image into an LSurface object
        spikeImage->LoadFromFile(img);

        // use colors on the top bottom and middle of legend
        ColorToNumber c2n(
            (SDL_Color){253, 231,  37}, // max color
            (SDL_Color){ 33, 145, 140}, // mid color
            (SDL_Color){ 68,   1,  84}  // min color
        );

        // find the 0-1 value for each cell in the figure
        std::vector<std::vector<float>> vals = c2n.findValues(spikeImage, legend_map);
        int n;
        float tot = 0.f;
        float avg = 0.f;
        // convert values to lines of a text file for saving
        // this function will also calculate the total and average cell values
        std::vector<std::string> lines = ValuesToFileLines(vals, &n, &tot, &avg);
        
        // data save location
        std::string outputloc = CreateOutputLocationWithFilename(output_folder, img, ".txt");
        
        // output results
        if (printAny)
        {
            if (!printVerbose) n = img.size() * 2;
    
            // print filename
            for (int i = 0; i < n; ++i) std::cout << '=';
            std::cout << "\n\n" << img << "\n\n";
            for (int i = 0; i < n; ++i) std::cout << '-'; std::cout << "\n\n";
            
            // print all data in lines vector (this is what gets saved to file)
            if (printVerbose)
            {
                for (auto& s : lines) std::cout << s <<'\n';
                std::cout << "\n"; 
                for (int i = 0; i < n; ++i) std::cout << '-';
                std::cout << "\n\n"; 
            }
    
            // save data and output success/failure
            std::cout << "Saving to: " << outputloc << "...\n";
            if (WriteFile(outputloc, lines) == 0) std::cout << "Data Saved.\n\n";
            else std::cout << "Failed to save data!\n\n";
            
            for (int i = 0; i < n; ++i) std::cout << '='; std::cout << "\n\n";
        }
        // not print, save data without printing
        else WriteFile(outputloc, lines);


        /**
         * This is the code that handles saving the totals and averages to a spreadsheet.
         * The current filename is parsed for the animal and div numbers. This is used
         * to organize the animal into a hash table, which will be saved after the for
         * loop's completion
         */

        // parse the filename for identifying information
        std::string animalPlate;
        int div;
        Spreadsheet::ParseFilename(img, animalPlate, div);
        // add plate to spreadsheet
        spreadsheet.AddEntry(animalPlate, div, tot, avg);
    }

    // save spreadsheets
    spreadsheet.Save(output_folder + "totals.csv", output_folder + "averages.csv");

    spikeImage->free(); // cleanup

    return 0;
}