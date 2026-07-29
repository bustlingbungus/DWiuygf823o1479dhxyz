#include <SDL3_image/SDL_image.h>
#include <iostream>

#include "LSurface.hpp"



LSurface::LSurface()
{
    mSurface = nullptr;
    mWidth = 0;
    mHeight = 0;
    pixelData = {};
}


LSurface::~LSurface()
{
    free();
}


/**
 * Deallocates memory by calling `SDL_DestroySurface` on the internal surface reference,
 * and resetting all members.
 */
void LSurface::free()
{
    if (mSurface != nullptr)
    {
        SDL_DestroySurface(mSurface);
        mSurface = nullptr;
        mWidth = 0;
        mHeight = 0;
        pixelData.clear();
    }
}


/**
 * Loads an image from file and creates and SDL surface object.
 * Effectively initializes the LSurface with an image, pixel data, etc.
 * Deallocates all memory of any previously loaded surface.
 * 
 * \warning attempting to load a non-image or invalid image may lead to undefined behavior
 * 
 * \param filename the name of the image to load
 * 
 * \returns `true` for successful load, `false` in the event of error.
 */
bool LSurface::LoadFromFile(const std::string& filename)
{
    free(); // clear previous surface

    // load image from file
    SDL_Surface* loadedSurface = IMG_Load(filename.c_str());
    if (loadedSurface == nullptr)
    {
        std::cerr << "Unable to create texture from \'" << filename 
                  <<"\'! SDL Error: " << SDL_GetError() <<'\n'; 
    }
    else
    {
        // get surface dimensions
        mWidth = loadedSurface->w;
        mHeight = loadedSurface->h;
        mSurface = loadedSurface;
    }

    if (mSurface == nullptr) return false;
    // load surface's pixel colors into internal array
    LoadPixelData();
    return true;
}


/**
 * Width (px) of the loaded image
 */
int LSurface::Width()
{
    return mWidth;
}


/**
 * Height (px) of the loaded image
 */
int LSurface::Height()
{
    return mHeight;
}


/**
 * Reference to the raw `SDL_Surface` object
 */
SDL_Surface* LSurface::get()
{
    return mSurface;
}


/**
 * 2D array of all pixel colors in the image
 */
std::vector<std::vector<SDL_Color>> LSurface::Pixels()
{
    return pixelData;
}


/**
 * Color of the pixel (x, y) in the loaded image.
 * Returns black (0,0,0,0) if x and y are out of bounds
 */
SDL_Color LSurface::GetPixel(const int& x, const int& y)
{
    // indices out of bounds
    if (x < 0 || x >= pixelData.size() || y < 0 || y >= pixelData[0].size()) {
        return (SDL_Color){0, 0, 0, 0};
    }
    return pixelData[x][y];
}


/**
 * Retrieves pixel data from the loaded surface and stores color data in internal array
 */
void LSurface::LoadPixelData()
{
    // iterate through each pixel and get color
    pixelData = std::vector<std::vector<SDL_Color>>(mWidth, std::vector<SDL_Color>(mHeight));
    for (int x = 0; x < mWidth; ++x)
    {
        for (int y = 0; y < mHeight; ++y)
        {
            // read and assign pixel data
            SDL_Color pixel = {0, 0, 0, 0};
            if (!SDL_ReadSurfacePixel(mSurface, x, y, &pixel.r, &pixel.g, &pixel.b, &pixel.a))
            {
                std::cerr << "Could not load pixel data at (" <<x<<", "<<y
                          <<")! SDL_Error: "<< SDL_GetError() <<'\n';
            }
            pixelData[x][y] = pixel;
        }
    }
}