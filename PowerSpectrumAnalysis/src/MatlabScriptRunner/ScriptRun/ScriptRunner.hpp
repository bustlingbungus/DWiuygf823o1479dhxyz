#pragma once

#include <string>
#include <vector>


/**
 * This class takes in input parameters, and formats them into a string that contains
 * a MATLAB function call. 
 * 
 * Calling `Run` will invoke the MATLAB executable, using the generated function string 
 * as an argument. Basically, it runs the MATLAB algorrithm in a seperate process. 
 */
class ScriptRunner
{
    public:

        ScriptRunner(
            std::string ml_exec_path, 
            std::string inputfile, 
            float start_time, 
            float end_time, 
            float f_low, 
            float f_high, 
            float smoothing_window_size, 
            bool gen_figures, 
            float domain_min, 
            float domain_max, 
            float range_min, 
            float range_max, 
            float data_min, 
            float data_max, 
            float data_scale
        );

        ScriptRunner();
        ~ScriptRunner();

        int Run();

        std::string Name();

    private:

        std::vector<std::string> ml_cmdarr;

        std::string input_filename; 
};