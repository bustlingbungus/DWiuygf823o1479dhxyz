#include "utils.hpp"

#include <iomanip>
#include <sstream>
#include <algorithm>

#include "sys/systemcall.hpp"
#include "sys/file_access.hpp"
#include "ColorToNumber.hpp"



/**
 * bitshifts colors and combines bytes using arithmetic or, converting RGBA into 
 * a single 32 bit uint. If `includeAlpha` is set false, the upper 8 bits will be
 * 0 regardless of alpha value
 */
Uint32 rgba2hex(SDL_Color ColorRGBA, bool includeAlpha)
{
    Uint32 col = (Uint32(ColorRGBA.r)<<16) |
                 (Uint32(ColorRGBA.g)<<8) |
                 Uint32(ColorRGBA.b);
    if (includeAlpha) col |= (Uint32(ColorRGBA.a)<<24);
    return col;
}


/**
 * Converts a 32 bit integer to an rgba color struct.
 * Integer format should be:
 * - bits 0-7: blue
 * - bits 8-15: green
 * - bits 16-23: red
 * - bits 24-31: alpha
 */
SDL_Color hex2rgba(int colorHex)
{
    int a = (colorHex & 0xff000000) << 24;
    int r = (colorHex & 0x00ff0000) << 16;
    int g = (colorHex & 0x0000ff00) << 8;
    int b = (colorHex & 0x000000ff);
    return (SDL_Color){(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
}


/**
 * Inverse lerps between the rgb color components of a and b using x's rgb values,
 * then combines the three results for a single ilerp value.
 * 
 * \note Alter the `decision` parameter to change how the three ilerp values are combined.
 * \note - `ILERPCOLOR_Average` (default) will average the three results
 * \note - `ILERPCOLOR_Min` will return the smallest of the three values
 * \note - `ILERPCOLOR_Max` will return the greatest of the three values
 * \note - `ILERPCOLOR_Median` will return the median of the three values
 */
float InverseLerpColor(SDL_Color a, SDL_Color b, SDL_Color x, ILerpColorDecision decision)
{
    // take ilerp of rgb components indiviually
    float tr = InverseLerp(a.r, b.r, x.r);
    float tg = InverseLerp(a.g, b.g, x.g);
    float tb = InverseLerp(a.b, b.b, x.b);

    // use combination protocol to turn three values into one
    switch (decision)
    {
        default:
        case ILERPCOLOR_Average:
            return (tr + tg + tb) / 3.f;
        case ILERPCOLOR_Min:
            return std::min(std::min(tr, tg), tb);
        case ILERPCOLOR_Max:
            return std::max(std::max(tr, tg), tb);
        case ILERPCOLOR_Median:
        {
            std::vector<float> t = {tr, tg, tb};
            std::sort(t.begin(), t.end());
            return t[1];
        }
    }
}


/**
 * Converts a color to a string with format "[r,g,b]"
 */
std::string col2str(SDL_Color col)
{
    return "[" + std::to_string((int)col.r) + "," + 
                 std::to_string((int)col.g) + "," +
                 std::to_string((int)col.b) + "]";
}


/**
 * returns false if the greatest difference between any two of the r, g, and
 * b components is above `margin`. Otherwise, returns true (r g and b are all
 * similar, hence the color is grey)
 */
bool isGrey(SDL_Color col, int margin)
{
    if (abs(col.r - col.g) > margin || 
        abs(col.r - col.b) > margin || 
        abs(col.g - col.b) > margin)
        return false;
    return true;
}


/**
 * Returns x clamped between min and max
 */
float clampf(float x, float min, float max)
{
    if (x < min) return min;
    return (x > max) ? max : x;
}


/**
 * Creates a directory in the output folder for the file's animal, plate, and div. Returns a filename in 
 * the new output directory, with the same name as `filename` paramter, but using the provided extension
 * 
 * \param outputFolder The root directory to store output in
 * \param filename The absolute directory of the file, containing actual filename, and extension
 * \param newExtension The file extension the returned string should have 
 */
std::string CreateOutputLocationWithFilename(std::string outputFolder, std::string filename, std::string newExtension)
{
    // remove original extension
    int idx = filename.find(".png");
    // replace with desired file extension
    std::string res = filename.substr(0, idx) + newExtension;
    
    // find the last slash in the filename (separates the actual FILE name from the directory name)
    for (int i = idx - 1; i >= 0; --i)
    {
        // set result equal to the FILE name, and save index
        if (filename[i] == '\\' || filename[i] == '/')
        {
            idx = i;
            res = res.substr(i + 1, res.size() - i);
            break;
        }
    }

    // continuing from the index of the last slash, find 'N', which indicates the animal folder
    for (int i = idx; i >= 0; --i)
    {
        // N found
        if (filename[i] == 'N')
        {
            // create a new directory of the animal, plate, and div folders, but within the output folder
            std::string folder = filename.substr(i, idx - i + 1);
            CreateDirectory(outputFolder + folder);
            // resulting filename is the new output folder, plus the actual FILE name
            res = folder + res;
            break;
        }
    }

    // add the root output folder to the file name and return
    res = outputFolder + res;
    return res;
}


/**
 * Calculates the average and total values in vals, Formats them, and saves them to a vector 
 */
std::vector<std::string> ValuesToFileLines(std::vector<std::vector<float>>& vals, int* n, float* total, float* average)
{
    if (n != nullptr) *n = vals.size() * 6; // the number of '=' that should be printed, for formatting 
    std::vector<std::string> lines;

    // find total value in plot
    float tot;
    float avg;
    ColorToNumber::sumPlot(vals, tot, avg);
    if (total != nullptr) *total = tot;
    if (average != nullptr) *average = avg;

    // save plot's total value, rounded to 2 decimal places
    std::stringstream sstream;
    sstream << std::fixed<<std::setprecision(2) << tot <<"\n";
    lines.push_back("Plot Total:");
    lines.push_back(sstream.str());

    // add plot's average value, rounded to 2 decimal places
    sstream = std::stringstream();
    sstream << std::fixed<<std::setprecision(2) << avg <<"\n";
    lines.push_back("Plot Average:");
    lines.push_back(sstream.str());


    // save all individual values
    lines.push_back("Plot Values:");
    for (auto& r : vals)
    {
        // add each value to string stream, rounded to 2 decimal places
        std::stringstream vstream;
        for (auto& v : r)
        {
            vstream << std::fixed<<std::setprecision(2) << v << "  ";
        }
        lines.push_back(vstream.str());
    }
    
    return lines;
}


/**
 * Loads the file containing legend data for the figures. Saves data as a hash table where keys are colors,
 * and entries are corresponding values. If the file fails to load, an empty hash table is returned.
 */
std::unordered_map<int,float> GetLegend(std::string legend_file)
{
    std::unordered_map<int,float> legend_map;

    // use hard coded values that I got from a sample image
    legend_map = {
        {4723815,	0.052083},
        {4589660,	0.018229},
        {4656480,	0.031250},
        {4656737,	0.036458},
        {4657251,	0.039062},
        {4657508,	0.044271},
        {4724328,	0.054688},
        {4724585,	0.059896},
        {4724842,	0.062500},
        {4725356,	0.067708},
        {4725613,	0.070312},
        {4725870,	0.075521},
        {4726127,	0.078125},
        {4727155,	0.091146},
        {4523353,	0.010417},
        {4589402,	0.015625},
        {4590173,	0.023438},
        {4590430,	0.026042},
        {4658021,	0.046875},
        {4726640,	0.083333},
        {4726897,	0.085938},
        {4727668,	0.093750},
        {4727925,	0.098958},
        {4728182,	0.101562},
        {4728439,	0.106771},
        {4457046,	0.002604},
        {4523095,	0.007812},
        {4728952,	0.109375},
        {4729209,	0.114583},
        {4456788,	0.000000},
        {4663930,	0.117188},
        {4664442,	0.122396},
        {4664699,	0.125000},
        {4664956,	0.130208},
        {4665213,	0.132812},
        {4599934,	0.138021},
        {4600446,	0.140625},
        {4600703,	0.145833},
        {4600960,	0.148438},
        {4535681,	0.153646},
        {4536193,	0.156250},
        {4536450,	0.161458},
        {4471171,	0.164062},
        {4471427,	0.169271},
        {4471684,	0.171875},
        {4406660,	0.177083},
        {4406917,	0.179688},
        {4341637,	0.184896},
        {4341894,	0.187500},
        {4342150,	0.192708},
        {4276871,	0.195312},
        {4277383,	0.200521},
        {4212104,	0.203125},
        {4212360,	0.208333},
        {4147080,	0.210938},
        {4147337,	0.216146},
        {4082057,	0.218750},
        {4082313,	0.223958},
        {4082826,	0.226562},
        {4017546,	0.231771},
        {4017802,	0.234375},
        {3952522,	0.239583},
        {3952779,	0.242188},
        {3887499,	0.247396},
        {3887755,	0.250000},
        {3822475,	0.255208},
        {3822732,	0.257812},
        {3757452,	0.263021},
        {3757708,	0.265625},
        {3692684,	0.270833},
        {3692940,	0.273438},
        {3627660,	0.278646},
        {3627917,	0.281250},
        {3562637,	0.286458},
        {3562893,	0.289062},
        {3497613,	0.294271},
        {3497869,	0.296875},
        {3432589,	0.302083},
        {3432845,	0.304688},
        {3367565,	0.309896},
        {3367821,	0.312500},
        {3302542,	0.317708},
        {3302798,	0.320312},
        {3237518,	0.325521},
        {3237774,	0.328125},
        {3238030,	0.333333},
        {3172750,	0.335938},
        {3173006,	0.341146},
        {3107726,	0.343750},
        {3107982,	0.348958},
        {3042702,	0.351562},
        {3042958,	0.356771},
        {3043214,	0.359375},
        {2977934,	0.364583},
        {2978190,	0.367188},
        {2912654,	0.372396},
        {2912910,	0.375000},
        {2913166,	0.380208},
        {2847886,	0.382812},
        {2848142,	0.388021},
        {2782862,	0.390625},
        {2783118,	0.395833},
        {2783374,	0.398438},
        {2718094,	0.403646},
        {2718350,	0.406250},
        {2718606,	0.411458},
        {2653326,	0.414062},
        {2653582,	0.419271},
        {2588558,	0.427083},
        {2588814,	0.429688},
        {2523790,	0.437500},
        {2458766,	0.450521},
        {2459022,	0.453125},
        {2393998,	0.460938},
        {2328974,	0.468750},
        {2263949,	0.476562},
        {2264205,	0.481771},
        {2199181,	0.489583},
        {2199693,	0.497396},
        {2072967,	0.572917},
        {2458510,	0.445312},
        {16180768,	0.986979},
        {2523534,	0.434896},
        {16311841,	0.989583},
        {14017050,	0.934896},
        {2005898,	0.544271},
        {10541625,	0.856771},
        {10672695,	0.861979},
        {10344763,	0.854167},
        {2328718,	0.466146},
        {15852829,	0.979167},
        {2134924,	0.513021},
        {15197209,	0.963542},
        {2199948,	0.500000},
        {2264461,	0.484375},
        {15525147,	0.971354},
        {14869528,	0.955729},
        {16639781,	0.997396},
        {2393742,	0.458333},
        {16049694,	0.981771},
        {2199437,	0.492188},
        {15394074,	0.966146},
        {2070922,	0.536458},
        {14344985,	0.942708},
        {2329229,	0.473958},
        {15721756,	0.973958},
        {2070411,	0.528646},
        {13492509,	0.924479},
        {2588302,	0.421875},
        {16508707,	0.994792},
        {8705099,	0.815104},
        {2600321,	0.622396},
        {2468994,	0.614583},
        {8508237,	0.809896},
        {9164358,	0.825521},
        {2072712,	0.565104},
        {3914613,	0.682292},
        {4374129,	0.697917},
        {4768110,	0.708333},
        {4636783,	0.705729},
        {4505456,	0.700521},
        {4242802,	0.692708},
        {4045940,	0.684896},
        {9820224,	0.841146},
        {9360965,	0.830729},
        {2402947,	0.606771},
        {9688897,	0.838542},
        {2337155,	0.604167},
        {5949540,	0.744792},
        {5293162,	0.723958},
        {3323514,	0.658854},
        {3717495,	0.674479},
        {4177011,	0.690104},
        {4899181,	0.713542},
        {5030508,	0.716146},
        {6343264,	0.755208},
        {5818213,	0.739583},
        {5424489,	0.729167},
        {5161835,	0.721354},
        {3848822,	0.677083},
        {11394096,	0.877604},
        {11066164,	0.869792},
        {2205061,	0.588542},
        {3651704,	0.669271},
        {6080611,	0.747396},
        {3126396,	0.651042},
        {6540127,	0.760417},
        {6933851,	0.770833},
        {6211938,	0.752604},
        {6802524,	0.768229},
        {5686887,	0.736979},
        {5555560,	0.731771},
        {3520377,	0.666667},
        {3454585,	0.661458},
        {3257723,	0.653646},
        {3060604,	0.645833},
        {7393111,	0.783854},
        {7261784,	0.778646},
        {6671198,	0.763021},
        {2994813,	0.643229},
        {7589974,	0.786458},
        {7721044,	0.791667},
        {7852371,	0.794271},
        {7130458,	0.776042},
        {2929022,	0.638021},
        {2797695,	0.635417},
        {2731903,	0.630208},
        {8377166,	0.807292},
        {2534785,	0.619792},
        {8048977,	0.799479},
        {8180304,	0.802083},
        {8836425,	0.817708},
        {2666112,	0.627604},
        {9033032,	0.822917},
        {9492291,	0.833333},
        {2468738,	0.611979},
        {10016830,	0.846354},
        {10213692,	0.848958},
        {2271364,	0.598958},
        {11197490,	0.872396},
        {10869558,	0.864583},
        {2205317,	0.591146},
        {2271109,	0.596354},
        {11590959,	0.880208},
        {11722029,	0.885417},
        {11918891,	0.888021},
        {2139270,	0.583333},
        {12115497,	0.893229},
        {2139014,	0.580729},
        {12443430,	0.901042},
        {12246568,	0.895833},
        {2073223,	0.575521},
        {12771107,	0.908854},
        {12967969,	0.911458},
        {12640037,	0.903646},
        {2072968,	0.567708},
        {13164576,	0.916667},
        {13295903,	0.919271},
        {2072456,	0.559896},
        {2072201,	0.557292},
        {2006409,	0.552083},
        {13820443,	0.932292},
        {13689116,	0.927083},
        {2006153,	0.549479},
        {14213657,	0.940104},
        {2071178,	0.541667},
        {14541592,	0.947917},
        {2070667,	0.533854},
        {14672664,	0.950521},
        {2070155,	0.523438},
        {2069899,	0.520833},
        {2069644,	0.515625},
        {15066137,	0.958333},
        {2134668,	0.505208}
    };

    // // open file containing legend data
    // std::ifstream istream = OpenFile(legend_file);
    // if (istream)
    // {
    //     // read value from each line of input file
    //     std::string line;
    //     while (std::getline(istream, line))
    //     {
    //         int col;
    //         float t;
    
    //         // read in color and corresponding value from file line
    //         std::istringstream iss(line);
    //         iss >> col >> t;
    //         // save correlation in hash table, to be looked up later
    //         legend_map[col] = t;
    //     }
    // }

    return legend_map;
}


std::string FindGenotype(int animal, int plate)
{
    switch (animal)
    {
        case 102512:
            switch (plate)
            {
                case 39070: return "WT"; 
                case 39206: return "WT"; 
                case 39207: return "WT"; 
                case 39213: return "WT"; 
                case 39214: return "WT"; 
                case 39215: return "WT"; 
            }
            return "Unknown";
        case 102641:
            switch (plate)
            {
                case 39030: return "WT";
                case 39028: return "APPKO";
                case 39207: return "APPKO";
            }
            return "Unknown";
        case 102808:
            switch (plate)
            {
                case 39207: return "WT";
            }
            return "Unknown";
        case 102821:
            switch (plate)
            {
                case 39070: return "WT";
                case 39206: return "WT";
                case 39213: return "WT";
                case 39214: return "WT";
                case 39215: return "WT";
            }
            return "Unknown";
        case 103024:
            switch (plate)
            {
                case 39211: return "WT";
                case 39206: return "WT";
                case 39207: return "WT";
                case 39213: return "WT";
                case 39028: return "PSIKI";
                case 39030: return "PSIKI";
                case 39037: return "PSIKI";
                case 39070: return "PSIKI";
                case 39072: return "PSIKI";
                case 39076: return "PSIKI";
                case 39214: return "PSIKI";
                case 39215: return "PSIKI";
            }
            return "Unknown";
        case 103111:
            switch (plate)
            {
                case 39214: return "WT";
                case 39030: return "PSIKI";
                case 39206: return "PSIKI";
                case 39072: return "PSIKI";
                case 39070: return "PSIKI";
                case 39211: return "PSIKI";
                case 39037: return "PSIKI";
                case 39213: return "PSIKI";
                case 39215: return "PSIKI";
            }
            return "Unknown";
        case 103262:
            switch (plate)
            {
                case 39214: return "WT";
                case 39213: return "WT";
                case 39206: return "PSIKI";
                case 39072: return "PSIKI";
                case 39207: return "PSIKI";
                case 39215: return "PSIKI";
                case 39076: return "PSIKI";
                case 39037: return "PSIKI";
                case 39030: return "PSIKI";
                case 39211: return "PSIKI";
                case 39028: return "PSIKI";
            }
            return "Unknown";
        case 102948:
            switch (plate)
            {
                case 39206: return "APPKO";
                case 39030: return "APPKO";
                case 39070: return "APPKO";
                case 39076: return "APPKOPSIKI";
                case 39207: return "APPKOPSIKI";
                case 39213: return "APPKOPSIKI";
                case 39028: return "APPKOPSIKI";
                case 39214: return "APPKOPSIKI";
            }
            return "Unknown";
        case 103185:
            switch (plate)
            {
                case 39211: return "APPKO";
                case 39072: return "APPKO";
                case 39207: return "APPKO";
                case 39076: return "APPKO";
                case 39215: return "APPKO";
                case 39206: return "APPKO";
                case 39213: return "APPKOPSIKI";
                case 39028: return "APPKOPSIKI";
                case 39030: return "APPKOPSIKI";
                case 39037: return "APPKOPSIKI";
                case 39214: return "APPKOPSIKI";
            }
            return "Unknown";
        case 103281:
            switch (plate)
            {
                case 39028: return "APPKO";
                case 39207: return "APPKO";
                case 39214: return "APPKO";
                case 39211: return "APPKO";
                case 39215: return "APPKO";
                case 39072: return "APPKO";
                case 39030: return "APPKO";
                case 39206: return "APPKOPSIKI";
                case 39076: return "APPKOPSIKI";
                case 39213: return "APPKOPSIKI";
                case 39037: return "APPKOPSIKI";
            }
            return "Unknown";
        case 102312:
            switch (plate)
            {
                case 39206: return "APPKOPSIKI";
                case 39214: return "APPKOPSIKI";
                case 39215: return "APPKOPSIKI";
            }
            return "Unknown";
        case 103182:
            switch (plate)
            {
                case 39028: return "APPKOPSIKI";
                case 39030: return "APPKOPSIKI";
                case 39037: return "APPKOPSIKI";
                case 39070: return "APPKOPSIKI";
                case 39072: return "APPKOPSIKI";
                case 39076: return "APPKOPSIKI";
                case 39206: return "APPKOPSIKI";
                case 39207: return "APPKOPSIKI";
                case 39211: return "APPKOPSIKI";
                case 39213: return "APPKOPSIKI";
                case 39214: return "APPKOPSIKI";
                case 39215: return "APPKOPSIKI";
            }
            return "Unknown";
        case 103184:
            switch (plate)
            {
                case 39028: return "APPKOPSIKI";
                case 39030: return "APPKOPSIKI";
                case 39037: return "APPKOPSIKI";
                case 39070: return "APPKOPSIKI";
                case 39072: return "APPKOPSIKI";
                case 39076: return "APPKOPSIKI";
                case 39206: return "APPKOPSIKI";
                case 39207: return "APPKOPSIKI";
                case 39211: return "APPKOPSIKI";
                case 39213: return "APPKOPSIKI";
                case 39214: return "APPKOPSIKI";
                case 39215: return "APPKOPSIKI";
            }
            return "Unknown";
        default:
            return "Unknown";
    }
}