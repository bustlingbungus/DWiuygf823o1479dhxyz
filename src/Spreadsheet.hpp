#pragma once

#include <unordered_map>
#include <queue>
#include <string>



/**
 * Individual entry in the spreadsheet. Contains div number, total spike correlation,
 * and average spike correlation.
 */
struct SpreadsheetEntry
{
    /** Div number for this recording */
    int div = 0;
    /** Sum of all cell values in recording */
    float tot = 0.f;
    /** Average cell value in recording */
    float avg = 0.f;

    bool operator<(const SpreadsheetEntry& other) const;
};


/**
 * Contains a hash table with spreadsheet entries for each recording.
 * Entries are keyed by a string of the animal and plate numbers (which are unique),
 * and values are a priority queue of entries for each div (sorted lowest to highest div)
 */
class Spreadsheet
{
    public:

        Spreadsheet();
        ~Spreadsheet();

        static void ParseFilename(std::string& filename, std::string& animalPlate, int& div);

        void AddEntry(std::string animalPlate, int div, float total, float average);

        void Print();

        void Save(std::string tot_filename, std::string avg_filename);

    protected:

        /** Hash table containing entries for each recording */
        std::unordered_map<std::string,std::priority_queue<SpreadsheetEntry>> spreadsheet;
};