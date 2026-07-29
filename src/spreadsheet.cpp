#include "spreadsheet.hpp"

#include <iostream>
#include <algorithm>

#include "System/file_access.hpp"



Spreadsheet::Spreadsheet()
{

}


Spreadsheet::~Spreadsheet()
{
    free();
}


void Spreadsheet::free()
{
    // gen/animals
    for (auto& gt : data)
    {
        // animal/plates
        for (auto& an : gt.second) an.second.clear();
        gt.second.clear();
    }
    data.clear();
}


void Spreadsheet::AddEntry(Genotype genotype, int animal, int plate, int embryo, int div, std::vector<float>& values)
{
    if (data.find(genotype) == data.end()) {
        data.emplace(genotype, std::unordered_map<int,std::unordered_map<int,std::shared_ptr<Plate>>>());
    }

    auto animal_map = &data[genotype];
    if (animal_map->find(animal) == animal_map->end()) {
        animal_map->emplace(animal, std::unordered_map<int,std::shared_ptr<Plate>>());
    }

    auto plate_map = &(*animal_map)[animal];
    if (plate_map->find(plate) == plate_map->end()) {
        plate_map->emplace(plate, std::make_shared<Plate>());
    }

    auto pt = (*plate_map)[plate];
    pt->embryo = embryo;
    
    auto div_map = &pt->divs;
    if (div_map->find(div) == div_map->end()) {
        div_map->emplace(div, values);
    } else {
        std::cout << "Duplicate entry for N" << animal <<' '<< plate <<" div"<< div <<"!\n";
        (*div_map)[div] = values;
    }
}


void Spreadsheet::Save(std::string value_output, std::string bin_output, int num_bins)
{
    std::vector<std::string> lines;

    std::vector<std::vector<float>*> all_values;
    int mx_len = -1;

    std::vector<Genotype> genotypes(data.size());
    int i = 0;
    for (auto& gt : data) genotypes[i++] = gt.first;
    std::sort(genotypes.begin(), genotypes.end());

    std::string genotype_line = "";
    std::string animal_line = "";
    std::string plate_line = "";
    std::string div_line = "";
    for (Genotype& gen : genotypes)
    {
        auto animal_map = &data[gen];

        // format string for current genotype
        genotype_line += GenotypeToString(gen) + ",";
        // add space for each animal
        for (int i = 1, n = animal_map->size(); i < n; ++i) {
            genotype_line += ",";
        }

        // sort animals
        std::vector<int> animals(animal_map->size());
        i = 0;
        for (auto& an : *animal_map) animals[i++] = an.first;
        std::sort(animals.begin(), animals.end());

        for (int& anim : animals)
        {
            auto plate_map = &(*animal_map)[anim];

            // format string for current animal
            animal_line += "N" + std::to_string(anim) + ",";
            // add space for each plate
            for (int i = 1, n = plate_map->size(); i < n; ++i) {
                animal_line += ",";
                genotype_line += ",";
            }

            // sort plates
            std::vector<int> plates(plate_map->size());
            i = 0;
            for (auto& pt : *plate_map) plates[i++] = pt.first;
            std::sort(plates.begin(), plates.end());

            for (int& pt : plates)
            {
                auto div_map = &(*plate_map)[pt]->divs;
                int embryo = (*plate_map)[pt]->embryo;

                // format string for current plate
                plate_line += std::to_string(pt);
                if (embryo > 0) plate_line += " E" + std::to_string(embryo);
                plate_line += ",";
                // add space for each div
                for (int i = 1, n = div_map->size(); i < n; ++i) {
                    plate_line += ",";
                    animal_line += ",";
                    genotype_line += ",";
                }


                // sort divs
                std::vector<int> divs(div_map->size());
                i = 0;
                for (auto& div : *div_map) divs[i++] = div.first;
                std::sort(divs.begin(), divs.end());

                for (int& div : divs)
                {
                    std::vector<float>* vals = &(*div_map)[div];
                    if ((int)vals->size() > mx_len) mx_len = vals->size();

                    // format string for current div
                    div_line += "div" + std::to_string(div) + ",";
                    all_values.push_back(vals);
                }
            }
        }
    }

    lines.push_back(genotype_line);
    lines.push_back(animal_line);
    lines.push_back(plate_line);
    lines.push_back(div_line);



    for (int i = 0; i < mx_len; ++i)
    {
        std::string vline = "";
        for (auto& val : all_values)
        {
            if (i < val->size()) vline += std::to_string((*val)[i]) + ",";
            else vline += ",";
        }
        lines.push_back(vline);
    }

    WriteFile(value_output, lines);



    std::vector<std::vector<int>> frequencies(all_values.size());

    for (int i = 0; i < all_values.size(); ++i)
    {
        std::vector<float>* vals = all_values[i];
        frequencies[i] = std::vector<int>(num_bins, 0);

        for (float& v : *vals)
        {
            int bin = int(float(num_bins-1) * v);
            if (bin >= 0 && bin < num_bins)
            {
                ++frequencies[i][bin];
            }
        }
    }

    lines.clear();
    lines = {
        "," + genotype_line,
        "," + animal_line,
        "," + plate_line,
        "Bin, " + div_line,
    };

    float bstep = 1.f / (float)num_bins;
    for (int i = 0; i < num_bins; ++i)
    {
        float bin = ((float)i + 0.5f) * bstep;
        std::string fline = std::to_string(i + 1) + ": "+ RemoveTrailingZeroes(std::to_string(bin)) + " +/- " + RemoveTrailingZeroes(std::to_string(bstep / 2.f)) + ",";
        
        for (auto& freq : frequencies)
        {
            fline += std::to_string(freq[i]) + ",";
        }
        lines.push_back(fline);
    }

    WriteFile(bin_output, lines);
}