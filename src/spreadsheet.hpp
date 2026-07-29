#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include "utils.hpp"

class Spreadsheet
{
    public:

        Spreadsheet();
        ~Spreadsheet();

        void free();

        void AddEntry(Genotype genotype, int animal, int plate, int embryo, int div, std::vector<float>& values);

        void Save(std::string value_output, std::string bin_output, int num_bins);

    protected:

        class Plate
        {   
            public:

                Plate();
                ~Plate();

                void free();

                int embryo = -1;
                std::unordered_map<int,std::vector<float>> divs;
        };

        std::unordered_map<
            Genotype,               // genotype
            std::unordered_map<int, // animal
            std::unordered_map<int, // plate
            std::shared_ptr<Plate>  // div and values
        >>> data;
};
