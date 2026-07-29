#pragma once

#include <string>
#include <vector>

class ScriptRunner; // forwards declaration


/**
 * Structure for loading analysis parameters from file, and saving to file
 * uses saved parameters to generate runners that execute MATLAB analysis.
 */
class ScriptConfig
{
    public:

        ScriptConfig();
        ~ScriptConfig();

        std::vector<ScriptRunner> GenerateRunners();

        void ShowParams();
        void EditParams();

        void ShowFiles();
        void EditFiles();
        
        void Find(bool log = true);
        
        void Load();
        void Save();


    private:

        /** Path to the MATLAB executable */
        std::string ml_exec_path;
        /** Directory to search for .h5 input files in */
        std::string base_data_path;

        /** Array of detected input files */
        std::vector<std::string> input_files;
        /** Time (in seconds) in recording to start reading data */
        float start_time;
        /** Time (in seconds) in recording to stop reading data */
        float end_time;
        /** Minimum frequency not being filtered out */
        float f_low;
        /** Maximum frequency not getting filtered out */
        float f_high;
        /** Size of moving average filter window */
        float smoothing_window_size;
        /** True to generate plots and figures for data */
        bool gen_figures;
        /** Start value for x axis */
        float domain_min;
        /** End value for x axis */
        float domain_max;
        /** Start value for y axis */
        float range_min;
        /** End value for y axis */
        float range_max;
        /** Graphs do not show data lower than this value */
        float data_min;
        /** Graphs do not show data higher than this value */
        float data_max;
        /** Linear scale all power data is multiplied by */
        float data_scale;

        void generate_default_settings();
};
