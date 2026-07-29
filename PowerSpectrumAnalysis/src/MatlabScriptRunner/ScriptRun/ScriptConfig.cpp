#include "ScriptConfig.hpp"
#include "ScriptRunner.hpp"
#include "../System/file_access.hpp"
#include "../Terminal/mlsr_glob.hpp"
#include "../Utils.hpp"

#include <iostream>



ScriptConfig::ScriptConfig()
{
    // default settings
    ml_exec_path = "None";
    base_data_path = "None";
    start_time = 120.f;
    end_time = 130.f;
    f_low = 1.f;
    f_high = 4.f;
    smoothing_window_size = 10;
    gen_figures = false; 
    domain_min = 0.f;
    domain_max = 20.f;
    range_min = 60.f;
    range_max = 165.f;
    data_min = 0.f;
    data_max = -1.f;
    data_scale = 1.f;
}

ScriptConfig::~ScriptConfig()
{
    ml_exec_path.clear();
    base_data_path.clear();
    input_files.clear();
}


/**
 * Generates an array of script runner objects using stored configurations.
 * Each runner will have a unique filename, and the sane configuration otherwise.
 * The returned array will have one unique entry for every file found the base data path.
 */
std::vector<ScriptRunner> ScriptConfig::GenerateRunners()
{
    std::vector<ScriptRunner> runners(input_files.size());

    for (int i = 0; i < (int)input_files.size(); ++i)
    {
        // generate script runner with current input file
        runners[i] = ScriptRunner(
            ml_exec_path,
            input_files[i],
            start_time,
            end_time,
            f_low,
            f_high,
            smoothing_window_size,
            gen_figures,
            domain_min,
            domain_max,
            range_min,
            range_max,
            data_min,
            data_max,
            data_scale
        );
    }

    return runners;
}

#pragma region Parameters


/**
 * Prints Analysis paramters to cout
 */
void ScriptConfig::ShowParams()
{
    std::cout <<"\nMatlab Script Configuration:\n\n" <<
                "Time (s):\t" << start_time <<" - "<< end_time <<"\n\n"<<
                "Targeting Frequency (Hz):\t" << f_low <<" - "<< f_high <<"\n"<<
                "Smoothing window size:\t" << smoothing_window_size <<"\n\n"<<
                "Frequency domain (x axis):\t" << domain_min <<" - "<< domain_max <<"\n"<<
                "Power range (y axis):\t" << range_min <<" - "<< range_max <<"\n"<<
                "Data collection range:\t" << data_min <<" - "<< (data_max==-1.f? domain_max : data_max) <<"\n\n"<<
                "Data scale:\t" << data_scale <<"\n\n"<<
                (gen_figures ? "Generating figures\n\n" : "Not generating figures\n\n");
}


/**
 * Get new analysis parameters from cin. User can enter a response to change each
 * variable. THey can also enter an empty response to leave a variable unchanged.
 * 
 * \warning Entering a non-valid number for any numeric value will cause a fatal error
 */
void ScriptConfig::EditParams()
{
    std::string response;

    // start time
    std::cout <<"Enter start time (s). Current value:\t" << start_time <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) start_time = std::stof(response);
    
    // end time
    std::cout <<"Enter end time (s). Current value:\t" << end_time <<
    "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) end_time = std::stof(response);   
    
    // f low
    std::cout <<"Enter minimum frequency threshold (Hz). Current value:\t" << f_low <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) f_low = std::stof(response);
        
    // f high
    std::cout <<"Enter maximum frequency threshold (Hz). Current value:\t" << f_high <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) f_high = std::stof(response);
    
    // smoothing window
    std::cout <<"Enter smoothing window size. Current value:\t" << smoothing_window_size <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) smoothing_window_size = std::stof(response);
    
    // domain min
    std::cout <<"Enter the lower end of the domain (x axis). Current value:\t" << domain_min <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) domain_min = std::stof(response);

    // domain max
    std::cout <<"Enter the upper end of the domain (x axis). Current value:\t" << domain_max <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) domain_max = std::stof(response);
    
    // range min
    std::cout <<"Enter the lower end of the range (y axis). Current value:\t" << range_min <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) range_min = std::stof(response);

    // range max
    std::cout <<"Enter the upper end of the range (y axis). Current value:\t" << range_max <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) range_max = std::stof(response);
    
    // data min
    std::cout <<"Enter the lower end of the data collection range. Current value:\t" << data_min <<
                "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) data_min = std::stof(response);

    // data max
    std::cout <<"Enter the upper end of the data collection range. Current value:\t" << (data_max==-1.f? domain_max : data_max) <<
                "\nEnter \' \' to leave unchanged. Enter \'-1\' for no upper limit:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) data_max = std::stof(response);
    
    // data scale
    std::cout <<"Enter the linear data scale. Current value:\t" << data_scale <<
                "\nEnter \' \' to leave unchanged.:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) data_scale = std::stof(response);
    

    // gen figures
    std::cout << "Would you like the program to generate figures? (Y/n). Current value:\t"
              << (gen_figures ? "Y" : "n")
              << "\nEnter \' \' to leave unchanged:\t";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) gen_figures = YNresponse(response);

    // Save new parameters
    Save();
    std::cout << "Changes saved.\n\n";
}


#pragma endregion

/**
 * Shows the path to the matlab executable, and the path the algorithm looks for data files in.
 * Displays all .h5 files found be the algorithm.
 */
void ScriptConfig::ShowFiles()
{
    std::cout << "\nBase Data Path:\t" << base_data_path <<'\n'<<
                 "Matlab Executable Path:\t" << ml_exec_path <<
                 "\n\nDetected .h5 files:\n\n";

    for (std::string& file : input_files) std::cout << file <<'\n';
    std::cout << "\n";
}


/**
 * Edit file variables with string input. Filenames input by user are formatted to remove
 * leading/trailing whitespace, and to remove any slash at the end of the filename.
 * 
 * \note There is no check to make sure the submitted filenames actually exist. Invalid
 * filenames will not cause any fatal error, but will prevent the analysis from functioning.
 */
void ScriptConfig::EditFiles()
{
    std::string response;

    // base data path
    std::cout << "Enter the directory you would like to search for .h5 files. Current directory:\t" << base_data_path <<
                 "\nEnter \' \' to leave unchanged: ";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) base_data_path = FormatFilename(response, SLASHDIR_Forwards, false);
    
    // ml executable
    std::cout << "Enter the path of the matlab executable. Current path:\t" << ml_exec_path <<
                 "\nEnter \' \' to leave unchanged: ";
    std::getline(std::cin, response);
    if (HasNonWhitespace(response)) ml_exec_path = FormatFilename(response, SLASHDIR_Forwards, false);

    // Save new parameters, and try to find input files.
    Save();
    std::cout << "Changes saved.\n";
    Find();
}


#pragma region File Management

/**
 * Attempts to find .h5 files in the data path parameter. Searches the directory 
 * recursively to find all files. All reults are saved for later use when generating runners.
 */
void ScriptConfig::Find(bool log)
{
    if (base_data_path != "None")
    {
        if (log) std::cout << "Searching for .h5 files...\n";
        // use file recursive search to get .h5 files.
        // for some reason, some files are stored as .h5.txt, so blacklist .txt
        std::vector<std::string> whitelist = {".h5"};
        std::vector<std::string> blacklist = {".txt"};
        input_files = GetFilesRecursive(base_data_path, whitelist, blacklist);
        if (log) std::cout << (int)input_files.size() <<" files found.\n";
    }
    else if (log)
        std::cout << "Cannot search for files because \'base_data_path\' is not defined.\n";
}


/**
 * Read in parameters from `settings.txt`. If settings.txt does not exist in the current working
 * directory, default settings will be used. The default settings will be saved to a new 
 * `settings.txt` file in the current working directory.
 */
void ScriptConfig::Load()
{
    // open settings file
    std::ifstream settings = OpenFile("settings.txt");
    if (!settings || !settings.is_open()) // file not found/can't open
    {
        // generate default settings file
        std::cout << "Notice: Could not open file \'settings.txt\' in directory \'" 
                  << cwd 
                  << "\'.\nAutomatically generating settings. Enter \'config\' for more information.\n"; 
        generate_default_settings();
    }
    else
    {
        // iterate through each line.
        // When a label is found, it's respective value will be on the next line.
        // So, when a label is found, prepare its address to be loaded with the next value.
        
        std::string line;
        // pointers for each data type in config
        std::string* next_str = nullptr;
        bool* next_bool = nullptr;
        float* next_float = nullptr;

        // iterate through lines
        while (std::getline(settings, line))
        {
            // there is a vale ready to be loaded
            if (next_str != nullptr)
            {
                *next_str = line;
                next_str = nullptr;
            }
            else if (next_bool != nullptr)
            {
                *next_bool = line == "1" ? true : false;
                next_bool = nullptr;
            }
            else if (next_float != nullptr)
            {
                *next_float = std::stof(line);
                next_float = nullptr;
            }

            // check current line for labels
            else if (line == "ml_exec_path")
                next_str = &ml_exec_path;
            else if (line == "base_data_path")
                next_str = &base_data_path;
            else if (line == "start_time")
                next_float = &start_time;
            else if (line == "end_time")
                next_float = &end_time;
            else if (line == "f_low")
                next_float = &f_low;
            else if (line == "f_high")
                next_float = &f_high;
            else if (line == "smoothing_window_size")
                next_float = &smoothing_window_size;
            else if (line == "gen_figures")
                next_bool = &gen_figures;
            else if (line == "domain_min")
                next_float = &domain_min;
            else if (line == "domain_max")
                next_float = &domain_max;
            else if (line == "range_min")
                next_float = &range_min;
            else if (line == "range_max")
                next_float = &range_max;
            else if (line == "data_min")
                next_float = &data_min;
            else if (line == "data_max")
                next_float = &data_max;
            else if (line == "data_scale")
                next_float = &data_scale;
        }

        // use loaded data path to find input files
        Find(false);
    }
}


/**
 * Saves current config settings to `settings.txt` in the current working directory. 
 * if `settings.txt` does not exist, it will be created. If it does, it will be overwritten.
 */
void ScriptConfig::Save()
{
    // organize settings into array with labels
    std::vector<std::string> lines = 
    {
        "ml_exec_path", FormatFilename(ml_exec_path, SLASHDIR_Forwards, false),
        "base_data_path", FormatFilename(base_data_path, SLASHDIR_Forwards, false),
        "start_time", std::to_string(start_time),
        "end_time", std::to_string(end_time),
        "f_low", std::to_string(f_low),
        "f_high", std::to_string(f_high),
        "smoothing_window_size", std::to_string(smoothing_window_size),
        "gen_figures", gen_figures ? "1" : "0",
        "domain_min", std::to_string(domain_min),
        "domain_max", std::to_string(domain_max),
        "range_min", std::to_string(range_min),
        "range_max", std::to_string(range_max),
        "data_min", std::to_string(data_min),
        "data_max", std::to_string(data_max),
        "data_scale", std::to_string(data_scale)
    };

    // write settings array to file
    if (WriteFile("settings.txt", lines) < 0) {
        std::cerr << "Error 011: Failed to save settings to \'settings.txt\'\n";
    }
}


/**
 * Initializes config setting to default values, and saves values to `settings.txt` in the 
 * current working directory.
 */
void ScriptConfig::generate_default_settings()
{
    // initialize settings
    ml_exec_path = "None";
    base_data_path = "None";
    start_time = 120.f;
    end_time = 130.f;
    f_low = 1.f;
    f_high = 4.f;
    smoothing_window_size = 10;
    gen_figures = false; 
    domain_min = 0.f;
    domain_max = 20.f;
    range_min = 60.f;
    range_max = 165.f;
    data_min = 0.f;
    data_max = -1.f;
    data_scale = 1.f;

    // format parameters as an array with labels
    std::vector<std::string> lines = 
    {
        "ml_exec_path", ml_exec_path,
        "base_data_path", base_data_path,
        "start_time", std::to_string(start_time),
        "end_time", std::to_string(end_time),
        "f_low", std::to_string(f_low),
        "f_high", std::to_string(f_high),
        "smoothing_window_size", std::to_string(smoothing_window_size),
        "gen_figures", gen_figures ? "1" : "0",
        "domain_min", std::to_string(domain_min),
        "domain_max", std::to_string(domain_max),
        "range_min", std::to_string(range_min),
        "range_max", std::to_string(range_max),
        "data_min", std::to_string(data_min),
        "data_max", std::to_string(data_max),
        "data_scale", std::to_string(data_scale)
    };

    // write settings array to file
    if (WriteFile("settings.txt", lines) < 0) {
        std::cerr << "Error 010: Failed to write to \'settings.txt\'\n";
    }
}

#pragma endregion