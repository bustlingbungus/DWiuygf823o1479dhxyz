#include "spreadsheet.hpp"



Spreadsheet::Plate::Plate() 
{

}


Spreadsheet::Plate::~Plate()
{
    free();
}


/**
 * Free all entries indiv hash table
 */
void Spreadsheet::Plate::free()
{
    for (auto& d : divs) d.second.clear();
    divs.clear();
}