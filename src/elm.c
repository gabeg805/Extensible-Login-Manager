/* *****************************************************************************
 * 
 * Name:    elm.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display and run the Extensible Login Manager.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elm.h"

/* Private functions */
static void usage(void);

/* Private variables */
typedef struct
{
    int help;
    int verbose;
    int preview;
    int run;
    int logout;
} ElmOptions;

/* ************************************************************************** */
/* Display the Extensible Login Manager  */
int main(int argc, char **argv)
{
    /* Check if no arguments given */
    if (argc == 0) {
        usage();
        exit(ELM_EXIT_SUCCESS);
    }

    /* Command line options */
    static const  char   *shortopts  = "hvpr";
    static struct option  longopts[] = {
        { "help",    no_argument,       0, 'h' },
        { "verbose", no_argument,       0, 'v' },
        { "preview", optional_argument, 0, 'p' },
        { "run",     optional_argument, 0, 'r' },
        { "logout",  optional_argument, 0,  0 },
        { 0,         0,                 0,  0 }
    };

    /* Parse options */
    ElmOptions options = {0, 0, 0, 0, 0};
    int        index   = 0;
    int        c;

    while ((c=getopt_long(argc, argv, shortopts, longopts, &index)) != -1)
    {
        switch (c)
        {
        case 'h':
            usage();
            exit(ELM_EXIT_SUCCESS);

        case 'v':
            options.verbose = 1;
            elm_io_set_verbose(options.verbose);
            break;

        case 'p':
            options.preview = 1;
            sleep(3);
            break;

        case 'r':
            options.run = 1;
            break;

        case 0:
            options.logout = 1;
            elmprintf(LOG, "Logout needs to be implemented. EXiting.");
            exit(100);
            break;

        default:
            elmprintf(LOGERR, "Unknown option specified '%c'. Exiting", c);
            exit(ELM_EXIT_INVALID_OPTION);
        }
    }

    /* Run login manager */
    if (options.run) {
        ElmLoginManager *manager = elm_login_manager_new();
        manager->set_preview_mode(options.preview);
        return manager->run();
    }

    return 0;
}

/* ************************************************************************** */
/* Print program usage */
void usage(void)
{
    printf("Usage: %s [options] <args>\n", PROGRAM);
    printf("\n");
    printf("Options:\n");
    printf("    -h, --help\n");
    printf("        Print program usage.\n");
    printf("\n");
    printf("    -v, --verbose\n");
    printf("        Verbose output.\n");
    printf("\n");
    printf("    -p, --preview\n");
    printf("        Run login manager in Preview Mode (ignores X window setup).\n");
}
