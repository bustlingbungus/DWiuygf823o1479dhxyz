#include "ColorToNumber.hpp"

#include <iostream>
#include <cmath>

#include "utils.hpp"



/**
 * Use the colors from the top, bottom, and middle of the legen bar in the image, to help with
 * value extraction.
 */
ColorToNumber::ColorToNumber(SDL_Color maxColor, SDL_Color midColor, SDL_Color minColor)
: maxCol(maxColor), midCol(midColor), minCol(minColor)
{

}


ColorToNumber::ColorToNumber()
{
    maxCol = (SDL_Color){0, 0, 0};
    midCol = (SDL_Color){0, 0, 0};
    minCol = (SDL_Color){0, 0, 0};
}


ColorToNumber::~ColorToNumber()
{
    maxCol = (SDL_Color){0, 0, 0};
    midCol = (SDL_Color){0, 0, 0};
    minCol = (SDL_Color){0, 0, 0};
}


/**
 * Converts an image of a spike correlation plot to a 2D array of values. Entry [0][0] will be the first cell,
 * [2][1] will be the second cell of the third row, and so on. The legend argument should be a hash table whose
 * keys are colors (as integers), and values are the 0-1 value associated with the given key.
 * 
 * \param img The spike correlation plot image to analyze
 * \param legend_map Hash table representing values associated with each color
 */
std::vector<std::vector<float>> ColorToNumber::findValues(std::shared_ptr<LSurface> img, std::unordered_map<int,float>& legend_map)
{
    /**
     * The algorithm functions by finding the coordinates of the top cell, which is the only cell in its row.
     * It is assumed that the top cell will contain the first non-grey pixel in the image, when pixels are
     * read top to bottom.
     * 
     * Once the top pixel is found, the pixel width of the first row can be found by getting the number of pixels
     * between the x coordinate of the top cell, and the x coordinate of the first grey pixel to its right. Then
     * the pixel height of the top row can be found by finding the distance from that first grey pixel, to the first
     * non-grey pixel below it. 
     * 
     * With these cell dimensions, we can traverse to the next cells below the top cell. Using the same method as
     * before, we can then find the pixel width and height of the second column and second row. We repeat this proccess
     * to find the pixel widths and heights of every row and column in the figure.
     * 
     * With the coordinates of the top cell, cell dimensions, and the number of rows and columns, we can use depth
     * first search to traverse each cell in the image, getting a value from the color of each cell we visit.
     */


    // get pixel data from the image, and find the x, y pixel corrdinates of the first cell
    std::vector<std::vector<SDL_Color>> pxdat = img->Pixels();
    int x;
    int y;
    _find_first_cell(pxdat, x, y);


    // find the widths and heights of each row and column
    std::vector<int> widths;
    std::vector<int> heights;
    _find_cell_dimensions(pxdat, x, y, widths, heights);
    int n = widths.size();

    // initialize output vector
    std::vector<std::vector<float>> res = std::vector<std::vector<float>>(n);
    for (int i = 0; i < n; ++i)
    {
        // first row has one entry, each row has one more entry than the last
        // initialize values to -1, which indicates an unvisited cell
        res[i] = std::vector<float>(i + 1, -1.f);
    }

    // find the values of each cell using depth first search
    findValuesRecursive(pxdat, x, y, widths, heights, 0, 0, res, legend_map);

    return res;
}


/**
 * Finds the total value of entries in the 2D vector, as well as the average entry value
 */
void ColorToNumber::sumPlot(std::vector<std::vector<float>>& values, float& total, float& avg)
{
    total = 0.f;
    int num = 0;

    for (std::vector<float>& r : values)
    {
        num += r.size();
        for (float& v : r)
        {
            // ensure -1 unvisited cells do not contribute to total
            if (v >= 0.f) total += v;
        }
    }
    
    // average = total value / total population
    // num = 1830; // .5 * (60^2 + 60)
    avg = total / (float)num;
}


/**
 * Converts a color to value using the closest match on the legend_map
 */
float ColorToNumber::ColorToVal(SDL_Color col, std::unordered_map<int,float>& legend_map)
{
    // convert color to integer to use as key
    int hex = rgba2hex(col);

    // color exists in legend, use value
    if (legend_map.find(hex) != legend_map.end()) return legend_map[hex];

    // find the key in the legend closest to the input color
    // it will be used as the key for this color
    int best_col = -1;
    float best_dist = 9999999999999.f;
    for (auto& l : legend_map)
    {
        // convert key to rgb color
        SDL_Color lcol = hex2rgba(l.first);
        float dr = lcol.r - col.r;
        float dg = lcol.g - col.g;
        float db = lcol.b - col.b;
        // distance will be the magnitude of the difference vector, treating r,g,b as an x,y,z vector
        float dist = std::sqrt((dr * dr) + (dg * dg) + (db * db));
        if (dist < best_dist)
        {
            best_dist = dist;
            best_col = l.first;
        }
    }

    // ensure a god match was found
    if (best_col != -1) return legend_map[best_col];


    // if a match failed, just use the crappy ilerp method
    float t = InverseLerpColor(midCol, maxCol, col);
    // pixel is between mid and max color
    if (t >= 0.f) return Lerp(0.5f, 1.f, clampf(t, 0.f, 1.f));
    // else pixel is between min and mid color
    t = InverseLerpColor(minCol, midCol, col);
    return Lerp(0.f, 0.5f, clampf(t, 0.f, 1.f));
}


/**
 * Finds the coordinate of the top cell in the figure, assuming the top cell contains the first non-grey
 * pixel in the image
 */
void ColorToNumber::_find_first_cell(std::vector<std::vector<SDL_Color>>& pixelData, int& x, int& y)
{
    for (int i = 0; i < pixelData.size(); ++i)
    {
        for (int j = 0; j < pixelData[i].size(); ++j)
        {
            // non-grey pixel detected, return coordinates
            if (!isGrey(pixelData[i][j]))
            {
                x = i;
                y = j;
                return;
            }
        }
    }
}


/**
 * Interpret the values of each cell by traversing cells using dfs. dfs implemented using recursive calls
 * of this function
 */
void ColorToNumber::findValuesRecursive(std::vector<std::vector<SDL_Color>>& pixelData, int x, int y, std::vector<int>& widths, std::vector<int>& heights, int i, int j, std::vector<std::vector<float>>& res, std::unordered_map<int,float>& legend_map)
{
    // indices oob
    if (i < 0 || i >= res.size() || j < 0 || j >= res[i].size()) return;
    // cell visited already
    if (res[i][j] >= 0.f) return;
    // x/y oob
    if (x < 0 || x >= pixelData.size() || y < 0 || y >= pixelData[x].size()) return;
    
    SDL_Color px = pixelData[x][y];
    // non-data grey cell
    if (isGrey(px)) return;


    // save cell value
    res[i][j] = ColorToVal(px, legend_map);

    // down cell
    findValuesRecursive(pixelData, x, y + heights[i], widths, heights, i + 1, j, res, legend_map);
    // right cell
    findValuesRecursive(pixelData, x + widths[j], y, widths, heights, i, j + 1, res, legend_map);
}


/**
 * Finds the pixel width of each column and the pixel height of each row in the figure.
 * 
 * The cells along the plot's diagonal (the top of each column, and right cell of each row) are used
 * to determine the width and height of its column/row, since the cells of a row are all the same height,
 * and the cells of a column are all the same width.
 * 
 * Starting at (x,y), the top left pixel of a cell, we find x1, the first grey pixel to the right of x. The
 * column width is x1 - x. Then, from (x1, y), which is grey, we find y1, the first non-grey pixel below y.
 * The row height is y1 - y. Then, we have the width and height of the cell at (x, y), and (x1, y1) will be 
 * the origin of the next cell to find dimensions for.
 * 
 * All detected widths and heights are saved in the input vectors. Iteration is stopped when y1 goes out of bounds
 * before finding a non-grey pixel, meaning there are no more cells. Therefore, the size of the 
 * width and height vectors are also the number of rows and columns in the figure
 */
void ColorToNumber::_find_cell_dimensions(std::vector<std::vector<SDL_Color>>& pixelData, int x, int y, std::vector<int>& widths, std::vector<int>& heights)
{
    while (x < pixelData.size() && y < pixelData[x].size())
    {
        // iterate right to find a grey pixel to the left of x
        int i;
        for (i = x; i < pixelData.size(); ++i)
        {
            if (isGrey(pixelData[i][y]))
            {
                // first grey pixel found, width is difference
                widths.push_back(i - x);
                break;
            }
        }
        x = i; // bring x up to grey pixel
    
        // iterate down to find a non-grey pixel below y
        int j;
        for (j = y; j < pixelData[x].size(); ++j)
        {
            if (!isGrey(pixelData[x][j]))
            {
                // first non-grey found, height is the difference
                heights.push_back(j - y);
                break;
            }
        }
        y = j; // bring y down to non-grey pixel

        // x and y are now at the origin of the next cell. 
    }

    // ensure width and height are the same size
    if (heights.size() < widths.size()) heights.push_back(heights[heights.size()-1]);
}