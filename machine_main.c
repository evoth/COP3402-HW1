#include <stdlib.h>
#include <string.h>
#include "machine.h"
#include "utilities.h"

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        // Load and run program
        machine_init(argv[1]);
        machine_exec();
    }
    else if (argc == 3 && strcmp(argv[1], "-p") == 0)
    {
        // Ran with -p flag; print program
        machine_init(argv[2]);
        machine_print_program();
    }
    else
    {
        // Invalid arguments
        bail_with_error("Usage: %s vm_testN.bof\n       %s -p vm_testN.bof\n",
                        argv[0], argv[0]);
    }

    return 0;
}