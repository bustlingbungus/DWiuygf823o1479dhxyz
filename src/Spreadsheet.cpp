#include "Spreadsheet.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "sys/file_access.hpp"
#include "utils.hpp"



/** Sorts spreadsheet entries in ascending div order */
bool SpreadsheetEntry::operator<(const SpreadsheetEntry& other) const
{
    return div > other.div;
}



Spreadsheet::Spreadsheet()
{

}


Spreadsheet::~Spreadsheet()
{

}


/**
 * Using the filename, finds the animal number, plate number and embryo number, and div number.
 * Animal number, plate number, and embryo number are all saved in one string, i.e. "N123456-54321 E0".
 * div number is just saved as an integer.
 * 
 * If animal number or div cannot be found, `animalPlate` will be set to "None" and `div` will be 0.
 * 
 * \param filename The name of the file to be parsed
 * \param animalPlate The string that will contain the animal, plate, and embryo numbers
 * \param div Integer containing div number
 */
void Spreadsheet::ParseFilename(std::string& filename, std::string& animalPlate, int& div)
{
    // initialize values in case anima/div cannot be found
    animalPlate = "None";
    div = 0;

    // all current animal numbers begin with 'N10', and no current filenames contain
    // this substring except as animal number
    int idx = filename.find("N10");
    if (idx == (int)std::string::npos) return; // animal not found


    /**
     * All files are expected to be of the format ".../NAnimal/Plate Embryo/div"
     * Starting at the index of the 'N', there will be one slash, followed by plate and embryo
     * number, then another slash. We want to replace the first slash with a '-', for formatting, 
     * then keep the plate and embryo numbers in our string, then stop parsing at the second slash.
     * 
     * So, starting at the 'N', we increment our index until we have two slashes. We keep the index
     * of the first slash, to be replaced with a '-', and at the second slash, make animalPlate the 
     * substring from 'N' until the index of the second slash. 
     */


    int slsh_cnt = 0; // number of encountered slashes
    int dsh_idx = 0; // index to place a '-' at
    int i; // iterating index

    // start at 'N' and iterate until 2 slashes are found
    for (i = idx; i < filename.size(); ++i)
    {
        // slash detected
        if (filename[i] == '/' || filename[i] == '\\')
        {
            ++slsh_cnt;
            // second slash, save substring, and place dash
            if (slsh_cnt == 2)
            {
                animalPlate = filename.substr(idx, i - idx);
                animalPlate[dsh_idx] = '-';
                break;
            }
            else dsh_idx = i - idx; // first slash, save index
        }
    }

    std::string genotype = FindGenotype(std::stoi(animalPlate.substr(1, 6)), std::stoi(animalPlate.substr(8, 5)));
    animalPlate = genotype + "," + animalPlate;

    // Now, the string after i should be 'div', as per filename expectations
    // ensure this is true, then use the 2 characters after 'div' to find the div number
    if (filename.substr(i+1, 3) == "div")
    {
        std::string num = filename.substr(i+4, 2); // div number string
        if (!isdigit(num[1])) num = num[0]; // div is just one digit
        div = std::stoi(num); // convert div number string to integer
    }
}


/**
 * Converts input values to spreadheet entry, and adds entry to the hash table.
 */
void Spreadsheet::AddEntry(std::string animalPlate, int div, float total, float average)
{
    // create entry
    SpreadsheetEntry entry = {div, total, average};

    // animal plate is not part of spreadsheet
    if (spreadsheet.find(animalPlate) == spreadsheet.end())
    {
        // create new priority queue for first div, and add the new entry 
        // to the hash table
        std::priority_queue<SpreadsheetEntry> pq;
        pq.push(entry);
        spreadsheet.emplace(animalPlate, pq);
    }
    // push the new div into the existing priority queue
    else spreadsheet[animalPlate].push(entry);
}


/**
 * prints all animal, animal, plates, and divs in the spreadsheet, formatted with their respective values.
 */
void Spreadsheet::Print()
{
    for (auto& e : spreadsheet)
    {
        // print animal and plate number
        std::cout << e.first <<"\t\t";

        for (auto pq = e.second; !pq.empty(); pq.pop())
        {
            // print div number, along with total and average
            SpreadsheetEntry d = pq.top();
            std::cout << "div" << d.div 
                      <<" total: "<<std::fixed<<std::setprecision(2)<< d.tot 
                      <<" average: "<<std::fixed<<std::setprecision(2)<< d.avg <<"\t\t";
        }
        std::cout <<'\n';
    }
}


/**
 * Formats a csv file according to animal, plat, and div numbers. Saves all figure totals
 * to `tot_filename`, and all figure averages to `avg_filename`.
 * 
 * \warning Will produce undefined files if filenames do not end with `.csv`
 */
void Spreadsheet::Save(std::string tot_filename, std::string avg_filename)
{
    bool complete = true; // tracks if all divs have been saved

    // hash tables for totals and averages respectively 
    std::unordered_map<std::string,std::stringstream> tot_as_sstream;
    std::unordered_map<std::string,std::stringstream> avg_as_sstream;
    std::string div_line = " , ,"; // line containing all div numbers, in order
    int cdiv = 0; // current div number

    do
    {
        complete = true; // set to false if there is more data to add
        
        bool use_cdiv = false; // set to true if we use the current div number
        for (auto& e : spreadsheet) // iterate through spreadhseet to see if the current div is in use
        {
            if (!e.second.empty()) // still data to add, not complete
            {
                complete = false;
                // next entry for this plate is the current div number
                if (e.second.top().div == cdiv) 
                {
                    use_cdiv = true; // true, use current div
                    div_line += "div" + std::to_string(cdiv) + ","; // add cdiv to div line
                    break;
                }
            } 
        }

        // current div number contains data
        if (!complete && use_cdiv)
        {
            // iterate through spreadsheet to see which plates have data for the current div
            for (auto& e : spreadsheet)
            {
                // get references to stringstreams for the current animal-plate entries
                auto tstream = &tot_as_sstream[e.first];
                auto astream = &avg_as_sstream[e.first];
    
    
                // next div for current plate is the current div, add data
                if (!e.second.empty() && e.second.top().div == cdiv)
                {
                    // add average and total to their respective streams
                    (*tstream) <<std::fixed<<std::setprecision(2)<<e.second.top().tot;
                    (*astream) <<std::fixed<<std::setprecision(2)<<e.second.top().avg;
                    // remove div from plate when done. loop is to handle duplicate div entries
                    while (!e.second.empty() && e.second.top().div == cdiv) e.second.pop();
                }
                else // plate not using cdiv, add empty cell
                {
                    (*tstream) << " ";
                    (*astream) << " ";
                }
    
                // terminate cell
                (*tstream) << ",";
                (*astream) << ",";
            }
        }

        ++cdiv; // continue to next div number

    } while (!complete); // exit loop when no plate has any remaining divs to enter


    // save totals
    std::vector<std::string> lines;
    lines.push_back(div_line); // div label is the first line
    for (auto& e : tot_as_sstream)
    {
        // for each entry, add the animal plate title, then all the csv values
        std::string l = e.first + "," + e.second.str();
        lines.push_back(l);
    }
    // write lines to file
    WriteFile(tot_filename, lines);
    
    
    // save averages
    lines.clear();
    lines.push_back(div_line);
    for (auto& e : avg_as_sstream)
    {
        std::string l = e.first + "," + e.second.str();
        lines.push_back(l);
    }
    WriteFile(avg_filename, lines);
}