#include "ScriptRunner.hpp"
#include "../System/systemcall.hpp"

#include <iostream>



ScriptRunner::ScriptRunner(
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
)
: input_filename(inputfile)
{
    // format the input parameters into a function call for MATLAB
    std::string func_str = "\"cmd_MagSpectrumBandpass(\'" + // MATLAB function that will be called
                            input_filename + "\', " +
                            std::to_string(start_time) + ", " +
                            std::to_string(end_time) + ", " +
                            std::to_string(f_low) + ", " +
                            std::to_string(f_high) + ", " +
                            std::to_string(smoothing_window_size) + ", " +
                            (gen_figures ? "1" : "0") + ", " +
                            std::to_string(domain_min) + ", " +
                            std::to_string(domain_max) + ", " +
                            std::to_string(range_min) + ", " +
                            std::to_string(range_max) + ", " +
                            std::to_string(data_min) + ", " +
                            std::to_string(data_max) + ", " +
                            std::to_string(data_scale)
                            + ")\"";

    // add the function string as a command line argument
    // MATLAB executatble will be called. '-batch' is used to prevent MATLAB gui from opening
    ml_cmdarr = {ml_exec_path, "-batch", func_str};
}



ScriptRunner::ScriptRunner()
{

}


ScriptRunner::~ScriptRunner()
{
    ml_cmdarr.clear();
}


/**
 * Runs the MATLAB executable with the `-batch` argument, and using the provided input
 * parameters to call a MATLAB function.
 * 
 * \returns 0 for success, 7 for failure
 */
int ScriptRunner::Run()
{
    // attempt to run generated command line
    if (RunCommands(ml_cmdarr[0], ml_cmdarr) != 0) {
        std::cerr << "Error 007: Failed to execute command \'" << ml_cmdarr[0] << "\'\n";
        return 7;
    }
    return 0;
}


/**
 * Returns the filename this runner is using
 */
std::string ScriptRunner::Name()
{
    return input_filename;
}