#include <iostream>
#include <string>
#include <vector>

#include "System/file_access.hpp"
#include "utils.hpp"
#include "spreadsheet.hpp"

int main()
{
    std::string input_dir = "F:/MarilynIkhane/Marilyn_MEA/Caden Analysis/heatmaps/";

    std::string value_output = "F:/MarilynIkhane/Marilyn_MEA/Caden Analysis/heatmaps/values.csv";
    std::string bin_output = "F:/MarilynIkhane/Marilyn_MEA/Caden Analysis/heatmaps/bins.csv";
    int num_bins = 100;

    // gather all files from input directory
    std::vector<std::string> files = GetFilesRecursive(input_dir, {".txt"});
    Spreadsheet spreadsheet;

    // add spreadsheet entry for each file
    for (auto& filename : files)
    {
        int animal;
        int plate;
        int embryo;
        int div;

        // get information from filename, use plate and animal to determine genotype
        GetAnimalInfo(filename, animal, plate, embryo, div);
        Genotype gen = FindGenotype(animal, plate);
        // get spike correlation values from file
        std::vector<float> values = GetCellValues(filename);

        // add data as spreadsheet entry
        spreadsheet.AddEntry(gen, animal, plate, embryo,  div, values);
    }

    // save spreadsheet to file
    spreadsheet.Save(value_output, bin_output, num_bins);

    return 0;
}