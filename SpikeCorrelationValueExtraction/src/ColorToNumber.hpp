#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "sdl/LSurface.hpp"



/**
 * Tool for converting a spike correlation image to a 2D array of values. Contains tools for 
 * finding the average and total values of the returned values.
 */
class ColorToNumber
{
    public:
    
        ColorToNumber(SDL_Color maxColor, SDL_Color midColor, SDL_Color minColor);

        ColorToNumber();
        ~ColorToNumber();

        std::vector<std::vector<float>> findValues(std::shared_ptr<LSurface> img, std::unordered_map<int,float>& legend_map);

        static void sumPlot(std::vector<std::vector<float>>& values, float& total, float& avg);

    protected:

        SDL_Color maxCol; // color at the top of the legend
        SDL_Color midCol; // color in the middle of the legend
        SDL_Color minCol; // color at the bottom of the legend

        float ColorToVal(SDL_Color col, std::unordered_map<int,float>& legend_map);

    private:

        void _find_first_cell(std::vector<std::vector<SDL_Color>>& pixelData, int& x, int& y);

        void findValuesRecursive(std::vector<std::vector<SDL_Color>>& pixelData, int x, int y, std::vector<int>& widths, std::vector<int>& heights, int i, int j, std::vector<std::vector<float>>& res, std::unordered_map<int,float>& legend_map);

        void _find_cell_dimensions(std::vector<std::vector<SDL_Color>>& pixelData, int x, int y, std::vector<int>& widths, std::vector<int>& heights);
};