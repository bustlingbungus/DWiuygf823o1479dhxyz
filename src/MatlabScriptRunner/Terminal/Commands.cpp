#include "Commands.hpp"
#include "mlsr_glob.hpp"
#include "../ScriptRun/ScriptConfig.hpp"
#include "../ScriptRun/ScriptRunner.hpp"

#include <iostream>
#include <ctime>



/**
 * Searches for and executes commands recognized by the program
 * 
 * \return result of executed command.
 * \return 3 if command is unrecognized. 
 */
int BuiltinCommands()
{
    // take first input as the command
    std::string cmd = cmdarr[0];
    if (cmd == "exit") return __exit_command();
    if (cmd == "help") return __help();
    if (cmd == "config") return __config();
    if (cmd == "run") return __run_scripts();
    if (cmd == "find") return __find();

    // builtin command not found
    return 3;
}


/**
 * Logs error for unrecognized command
 */
void HandleUnrecognizedCommand()
{
    std::cerr << "Error 003: Unrecognized command \'" << cmdarr[0] << "\'. Type \'help\' for a list of commands.\n";
}


/**
 * terminates the program with exit code 0
 */
int __exit_command()
{
    // validate exit command
    if (cmdarr.size() > 1) {
        std::cerr << "Error 002: Unrecognized argument \'" << cmdarr[1] << "\' for \'exit\'\n";
        return 2;
    }
    
    // exit the program
    __mlsr_close(0);
    return 0;
}


/**
 * Shows a list of all commands and optional flags
 */
int __help()
{
    // argument validation
    if (cmdarr.size() > 1) {
        std::cerr << "Error 002: Unrecognized argument \'" << cmdarr[1] << "\' for \'help\'\n";
        return 2;
    }
    
    std::cout <<"\nWelcome to the Matlab script runner for MEA magnitude spectrum analysis.\n" << 
                "Here are a list of commands:\n\n" <<
                "\thelp\t: Shows this page of commands\n" << 
                "\texit\t: Exits the program\n\n" <<
                "\tconfig\t: View/edit settings for data analysis\n" <<
                "\t<optional config flags>\n" <<
                "\t -edit, -e\t: Edit settings\n" <<
                "\t -show, -s\t: Show current config values.\n" <<
                "\t -files, -f\t: Lists the files that will be analyzed when \'run\' is executed.\n" <<
                "\t -param, -p\t: Shows the parameters for data analysis\n\n" <<
                "\trun\t: Runs matlab scripts\n" <<
                "\tfind\t: Finds .h5 files in the specified directory\n\n";
                
    return 0;
}


/**
 * Allows user to show and edit script runner parameters. Behavior determined using -f -s and 
 * -e flags as arguments
 */
int __config()
{
    // flags to know what behavior to use
    int edit_idx = -1;
    int show_idx = -1;
    int file_idx = -1;
    int param_idx = -1;

    // iterate through arguments to look for flags
    for (int i = 1; i < (int)cmdarr.size(); ++i)
    {
        std::string arg = cmdarr[i];
        // show argument
        if (arg == "-show" || arg == "-s")
        {
            if (show_idx != -1) {
                std::cerr << "Error 002: Too many show flags.\n";
                return 2;
            } else show_idx = i;
        }

        // edit argument
        else if (arg == "-edit" || arg == "-e")
        {
            if (edit_idx != -1) {
                std::cerr << "Error 002: Too many edit flags.\n";
                return 2;
            } else edit_idx = i;
        }

        // flag to show/edit files page
        else if (arg == "-files" || arg == "-f")
        {
            if (file_idx != -1) {
                std::cerr << "Error 002: Too many file flags.\n";
                return 2;
            } else if (param_idx != -1) {
                std::cerr << "Error 011: Incompatible flags \'"<<arg<<"\' and \'"<<cmdarr[param_idx]<<"\'\n";
                return 2;
            } else file_idx = i;
        }

        // flag to show/edit parameter page
        else if (arg == "-param" || arg == "-p")
        {
            if (param_idx != -1) {
                std::cerr << "Error 002: Too many parameter flags.\n";
                return 2;
            } else if (file_idx != -1) {
                std::cerr << "Error 011: Incompatible flags \'"<<arg<<"\' and \'"<<cmdarr[file_idx]<<"\'\n";
                return 2;
            } else param_idx = i;
        }

        // unknown flag
        else {
            std::cerr << "Error 002: Unrecognized argument \'" << arg << "\' for \'config\'\n";
            return 2;
        }
    }

    // edit
    if (edit_idx > 0)
    {
        // edit files
        if (file_idx > 0) scriptConfig->EditFiles();
        // edit params (default)
        else scriptConfig->EditParams();
    }
    // show
    if (show_idx > 0 || edit_idx < 0)
    {
        if (file_idx > 0) scriptConfig->ShowFiles();
        else scriptConfig->ShowParams();

        if (show_idx < 0)
        {
            // let user know they can edit
            std::cout << "\nUse the \'-edit\' flag to edit this configuration. ";

            if (file_idx < 0) {
                std::cout << "(i.e., enter \"config -edit\")\n";
                std::cout << "Use the \'-files\' flag to view and/or edit file configurations.\n";
            }
            else std::cout << "(i.e., enter \"config -files -edit\")\n";
        }
    }

    return 0;
}


/**
 * Uses global config to generate an array of script runners, executes all script runners. 
 * Tracks how long it takes to run each script, and uses the average runtime to estimate
 * how much time remains to run the rest of the runners.
 */
int __run_scripts()
{
    // argument validation
    if (cmdarr.size() > 1) {
        std::cerr << "Error 002: Unrecognized argument \'" << cmdarr[1] << "\' for \'run\'\n";
        return 2;
    }

    // total time spent and current file being run
    float tot_time = 0.f;
    int c_num = 1;


    // get array of runners
    std::vector<ScriptRunner> runners = scriptConfig->GenerateRunners();
    for (ScriptRunner& runner : runners)
    {
        // begin timer for current runner
        std::cout << "\n================================================================================\n";
        clock_t begin_t = clock();
        std::cout << "file [ " << c_num <<" / "<< runners.size() <<" ]\n";
        std::cout << "\nRunning analysis for \'" << runner.Name() <<"\' ...\n\n";
        
        // run the external program
        int run_error = runner.Run();
        if (run_error != 0) return run_error;
        
        // find the runtime of analysis when done
        float time_s = float(clock() - begin_t) / 1000.0f;
        tot_time += time_s;
        // estimate how much time remains
        float avg_time = tot_time / c_num;
        float exp_remain = avg_time * float(runners.size() - c_num);


        // convert times in seconds into strings to display 
        std::string tot_str;
        if (tot_time <= 60.0f) tot_str = std::to_string(tot_time) + " seconds";
        else if (tot_time <= 3600.0f) tot_str = std::to_string(tot_time / 60.0f) + " minutes";
        else tot_str = std::to_string(tot_time / 3600.0f) + " hours";
        std::string exp_str;
        if (exp_remain <= 60.0f) exp_str = std::to_string(exp_remain) + " seconds";
        else if (exp_remain <= 3600.0f) exp_str = std::to_string(exp_remain / 60.0f) + " minutes";
        else exp_str = std::to_string(exp_remain / 3600.0f) + " hours";


        ++c_num; // increment current file index
        // runtime information
        std::cout << "\nAnalysis completed in " << time_s <<"s\n\n";
        std::cout << "Total time passed: " << tot_str << "\nExpected time remaining: " << exp_str <<"\n\n";
        std::cout << "\n================================================================================\n";
    }

    return 0;
}


/**
 * Finds .h5 files in the config's data directory
 */
int __find()
{
    // argument validation
    if (cmdarr.size() > 1) {
        std::cerr << "Error 002: Unrecognized argument \'" << cmdarr[1] << "\' for \'find\'\n";
        return 2;
    }

    scriptConfig->Find();

    return 0;
}