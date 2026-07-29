#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>



/**
 * Object to load an SDL surface from an image file. 
 * Stores a read only 2D array of pixels in the surface. 
 */
class LSurface
{
    public:

        LSurface();
        ~LSurface();

        void free();

        bool LoadFromFile(const std::string& filename);

        int Width();
        int Height();

        SDL_Surface* get();

        std::vector<std::vector<SDL_Color>> Pixels();
        SDL_Color GetPixel(const int& x, const int& y);

    protected:

        /** Reference to the actual surface object */
        SDL_Surface *mSurface;

        /** Width (px) of the image */
        int mWidth;
        /** Height (px) of the image */
        int mHeight;

        /** Color values for every pixel in the image */
        std::vector<std::vector<SDL_Color>> pixelData;

    private: 

        void LoadPixelData();
};