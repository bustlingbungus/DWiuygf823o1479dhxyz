#include "utils.hpp"

#include <sstream>
#include <iterator>
#include <iostream>

#include "System/file_access.hpp"


Genotype FindGenotype(int animal, int plate)
{
    switch (animal)
    {
        case 102512:
            switch (plate)
            {
                case 39070: return GENOTYPE_WT; 
                case 39206: return GENOTYPE_WT; 
                case 39207: return GENOTYPE_WT; 
                case 39213: return GENOTYPE_WT; 
                case 39214: return GENOTYPE_WT; 
                case 39215: return GENOTYPE_WT; 
            }
            return GENOTYPE_UNKNOWN;
        case 102641:
            switch (plate)
            {
                case 39030: return GENOTYPE_WT;
                case 39028: return GENOTYPE_APPKO;
                case 39207: return GENOTYPE_APPKO;
            }
            return GENOTYPE_UNKNOWN;
        case 102808:
            switch (plate)
            {
                case 39207: return GENOTYPE_WT;
            }
            return GENOTYPE_UNKNOWN;
        case 102821:
            switch (plate)
            {
                case 39070: return GENOTYPE_WT;
                case 39206: return GENOTYPE_WT;
                case 39213: return GENOTYPE_WT;
                case 39214: return GENOTYPE_WT;
                case 39215: return GENOTYPE_WT;
            }
            return GENOTYPE_UNKNOWN;
        case 103024:
            switch (plate)
            {
                case 39211: return GENOTYPE_WT;
                case 39206: return GENOTYPE_WT;
                case 39207: return GENOTYPE_WT;
                case 39213: return GENOTYPE_WT;
                case 39028: return GENOTYPE_PSIKI;
                case 39030: return GENOTYPE_PSIKI;
                case 39037: return GENOTYPE_PSIKI;
                case 39070: return GENOTYPE_PSIKI;
                case 39072: return GENOTYPE_PSIKI;
                case 39076: return GENOTYPE_PSIKI;
                case 39214: return GENOTYPE_PSIKI;
                case 39215: return GENOTYPE_PSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 103111:
            switch (plate)
            {
                case 39214: return GENOTYPE_WT;
                case 39030: return GENOTYPE_PSIKI;
                case 39206: return GENOTYPE_PSIKI;
                case 39072: return GENOTYPE_PSIKI;
                case 39070: return GENOTYPE_PSIKI;
                case 39211: return GENOTYPE_PSIKI;
                case 39037: return GENOTYPE_PSIKI;
                case 39213: return GENOTYPE_PSIKI;
                case 39215: return GENOTYPE_PSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 103262:
            switch (plate)
            {
                case 39214: return GENOTYPE_WT;
                case 39213: return GENOTYPE_WT;
                case 39206: return GENOTYPE_PSIKI;
                case 39072: return GENOTYPE_PSIKI;
                case 39207: return GENOTYPE_PSIKI;
                case 39215: return GENOTYPE_PSIKI;
                case 39076: return GENOTYPE_PSIKI;
                case 39037: return GENOTYPE_PSIKI;
                case 39030: return GENOTYPE_PSIKI;
                case 39211: return GENOTYPE_PSIKI;
                case 39028: return GENOTYPE_PSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 102948:
            switch (plate)
            {
                case 39206: return GENOTYPE_APPKO;
                case 39030: return GENOTYPE_APPKO;
                case 39070: return GENOTYPE_APPKO;
                case 39076: return GENOTYPE_APPKOPSIKI;
                case 39207: return GENOTYPE_APPKOPSIKI;
                case 39213: return GENOTYPE_APPKOPSIKI;
                case 39028: return GENOTYPE_APPKOPSIKI;
                case 39214: return GENOTYPE_APPKOPSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 103185:
            switch (plate)
            {
                case 39211: return GENOTYPE_APPKO;
                case 39072: return GENOTYPE_APPKO;
                case 39207: return GENOTYPE_APPKO;
                case 39076: return GENOTYPE_APPKO;
                case 39215: return GENOTYPE_APPKO;
                case 39206: return GENOTYPE_APPKO;
                case 39213: return GENOTYPE_APPKOPSIKI;
                case 39028: return GENOTYPE_APPKOPSIKI;
                case 39030: return GENOTYPE_APPKOPSIKI;
                case 39037: return GENOTYPE_APPKOPSIKI;
                case 39214: return GENOTYPE_APPKOPSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 103281:
            switch (plate)
            {
                case 39028: return GENOTYPE_APPKO;
                case 39207: return GENOTYPE_APPKO;
                case 39214: return GENOTYPE_APPKO;
                case 39211: return GENOTYPE_APPKO;
                case 39215: return GENOTYPE_APPKO;
                case 39072: return GENOTYPE_APPKO;
                case 39030: return GENOTYPE_APPKO;
                case 39206: return GENOTYPE_APPKOPSIKI;
                case 39076: return GENOTYPE_APPKOPSIKI;
                case 39213: return GENOTYPE_APPKOPSIKI;
                case 39037: return GENOTYPE_APPKOPSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 102312:
            switch (plate)
            {
                case 39206: return GENOTYPE_APPKOPSIKI;
                case 39214: return GENOTYPE_APPKOPSIKI;
                case 39215: return GENOTYPE_APPKOPSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 103182:
            switch (plate)
            {
                case 39028: return GENOTYPE_APPKOPSIKI;
                case 39030: return GENOTYPE_APPKOPSIKI;
                case 39037: return GENOTYPE_APPKOPSIKI;
                case 39070: return GENOTYPE_APPKOPSIKI;
                case 39072: return GENOTYPE_APPKOPSIKI;
                case 39076: return GENOTYPE_APPKOPSIKI;
                case 39206: return GENOTYPE_APPKOPSIKI;
                case 39207: return GENOTYPE_APPKOPSIKI;
                case 39211: return GENOTYPE_APPKOPSIKI;
                case 39213: return GENOTYPE_APPKOPSIKI;
                case 39214: return GENOTYPE_APPKOPSIKI;
                case 39215: return GENOTYPE_APPKOPSIKI;
            }
            return GENOTYPE_UNKNOWN;
        case 103184:
            switch (plate)
            {
                case 39028: return GENOTYPE_APPKOPSIKI;
                case 39030: return GENOTYPE_APPKOPSIKI;
                case 39037: return GENOTYPE_APPKOPSIKI;
                case 39070: return GENOTYPE_APPKOPSIKI;
                case 39072: return GENOTYPE_APPKOPSIKI;
                case 39076: return GENOTYPE_APPKOPSIKI;
                case 39206: return GENOTYPE_APPKOPSIKI;
                case 39207: return GENOTYPE_APPKOPSIKI;
                case 39211: return GENOTYPE_APPKOPSIKI;
                case 39213: return GENOTYPE_APPKOPSIKI;
                case 39214: return GENOTYPE_APPKOPSIKI;
                case 39215: return GENOTYPE_APPKOPSIKI;
            }
            return GENOTYPE_UNKNOWN;
        default:
            return GENOTYPE_UNKNOWN;
    }
}


std::string GenotypeToString(Genotype genotype)
{
    switch (genotype)
    {
        case GENOTYPE_WT: return "WT";
        case GENOTYPE_APPKO: return "APPKO";
        case GENOTYPE_PSIKI: return "PSIKI";
        case GENOTYPE_APPKOPSIKI: return "APPKOPSIKI";
        case GENOTYPE_UNKNOWN:
        default:
            return "Unknown";
    }
}


void GetAnimalInfo(std::string filename, int& animal, int& plate, int& embryo, int& div)
{
    animal = -1;
    int aidx = filename.find("N10");
    if (aidx != std::string::npos && aidx <= filename.size() - 7)
    {
        animal = 0;
        std::string num = filename.substr(aidx + 1, 6);
        for (char& ch : num) {
            if (!std::isdigit(ch)) { animal = -1; break; }
        }
        if (animal != -1) animal = std::stoi(num);
    }

    plate = -1;
    if (aidx != std::string::npos)
    {
        int sepidx = -1;
        for (int i = aidx + 7; i < filename.size() - 6; ++i)
        {
            if (filename[i] == '/' || filename[i] == '\\')
            {
                sepidx = i + 1;
                break;
            }
        }

        if (sepidx != -1)
        {
            plate = 0;
            std::string num = filename.substr(sepidx, 5);
            for (char& ch : num) {
                if (!std::isdigit(ch)) { plate = -1; break; }
            }
            if (plate != -1) plate = std::stoi(num);
        }
    }

    embryo = -1;
    if (aidx != std::string::npos)
    {
        int eidx = -1;
        for (int i = aidx + 12; i < filename.size() - 3; ++i)
        {
            if (filename[i] == 'E')
            {
                eidx = i + 1;
                break;
            }
        }

        if (eidx != -1)
        {
            std::string num = filename.substr(eidx, 2);
            if (std::isdigit(num[0]))
            {
                if (!std::isdigit(num[1])) num = num.substr(0, 1);
                embryo = std::stoi(num);
            }
        }
    }


    div = -1;
    int didx = filename.find("div");
    if (didx != std::string::npos && didx <= filename.size() - 5)
    {
        std::string num = filename.substr(didx + 3, 2);
        if (std::isdigit(num[0]))
        {
            if (!std::isdigit(num[1])) num = num.substr(0, 1);
            div = std::stoi(num);
        }
    }
}


std::vector<float> GetCellValues(std::string filename)
{
    std::vector<float> values;

    std::ifstream f = OpenFile(filename);
    if (f && f.is_open())
    {
        std::string line;
        bool read = false;
        while (std::getline(f, line))
        {
            if (line.find("Plot Values:") != std::string::npos) read = true;
            else if (read)
            {
                std::istringstream ss(line);
                std::copy(std::istream_iterator<float>(ss),
                          std::istream_iterator<float>(),
                          std::back_inserter(values)
                );
            }
        }
    }

    return values;
}


std::string RemoveTrailingZeroes(std::string numstr)
{
    int len = 0;
    for (int i = 0; i < numstr.size(); ++i)
    {
        if (numstr[i] != '0') len = i + 1;
    }
    return numstr.substr(0, len);
}