#include "Terminal/mlsr_glob.hpp"
#include "Terminal/IO.hpp"
#include "Terminal/Commands.hpp"

int main()
{
    // initialize program
    int init_res = __mlsr_init();
    if (init_res) __mlsr_close(init_res);

    while (true)
    {
        // get user input and parse into commands
        GetUserCommand();
        ParseInputBuffer();

        if (cmdarr.size() > 0)
        {
            // execute user commands
            int builtin_feedback = BuiltinCommands();
            if (builtin_feedback == 3)
            {
                HandleUnrecognizedCommand();
            }
        }

    }

    return 0;
}