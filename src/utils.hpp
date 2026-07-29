#pragma once

#include <string>
#include <vector>


enum Genotype
{
    GENOTYPE_WT,
    GENOTYPE_APPKO,
    GENOTYPE_PSIKI,
    GENOTYPE_APPKOPSIKI,
    GENOTYPE_UNKNOWN
};


Genotype FindGenotype(int animal, int plate);

std::string GenotypeToString(Genotype genotype);

void GetAnimalInfo(std::string filename, int& animal, int& plate, int& embryo, int& div);

std::vector<float> GetCellValues(std::string filename);

std::string RemoveTrailingZeroes(std::string numstr);