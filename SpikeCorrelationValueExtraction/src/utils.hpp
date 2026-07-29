#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <vector>
#include <unordered_map>



/**
 * Protocol for how Ilerp value should be determined for `InverseLerpColor`
 */
enum ILerpColorDecision
{
    ILERPCOLOR_Average,
    ILERPCOLOR_Min,
    ILERPCOLOR_Max,
    ILERPCOLOR_Median
};



/**
 * Linear interpolation between a and b
 */
template <typename Ta, typename Tb, typename Tt>
inline Ta Lerp(Ta a, Tb b, Tt t)
{
    return (Ta)(a + (t * (b - a)));
}


/**
 * Find the interpolator t, used to produce x, with linear interpolation form a to b.
 * i.e., `x = Lerp(a, b, x)`.
 * 
 * \note if b == a, `0` is returned.
 */
template <typename Tr = float, typename Ta, typename Tb, typename Tx>
float InverseLerp(Ta a, Tb b, Tx x)
{
    if (b == a) return (Tr)0;
    return (Tr)(x - a) / (Tr)(b - a);
}


/**
 * Returns x as a positive value
 */
template <typename T>
inline T abs(T x)
{
    return x < (T)0 ? -x : x;
}


Uint32 rgba2hex(SDL_Color ColorRGBA, bool includeAlpha = false);

SDL_Color hex2rgba(int colorHex);

float InverseLerpColor(SDL_Color a, SDL_Color b, SDL_Color x, ILerpColorDecision decision = ILERPCOLOR_Average);

std::string col2str(SDL_Color col);

bool isGrey(SDL_Color col, int margin = 5);

float clampf(float x, float min, float max);

std::string CreateOutputLocationWithFilename(std::string outputFolder, std::string filename, std::string newExtension);

std::vector<std::string> ValuesToFileLines(std::vector<std::vector<float>>& vals, int* n = nullptr, float* total = nullptr, float* average = nullptr);

std::unordered_map<int,float> GetLegend(std::string legend_file);

std::string FindGenotype(int animal, int plate);