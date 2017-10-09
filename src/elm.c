/* *****************************************************************************
 * 
 * Name:    elm.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the Extensible Login Manager.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elm.h"

/* Private functions */
static void usage(void);

/* ************************************************************************** */
/* Display the Extensible Login Manager  */
int main(int argc, char **argv)
{
    /* Check if no arguments given */
    if (argc == 0) {
        usage();
        exit(0);
    }

    /* Command line options */
    static char *shortopts = "hvpr";
    static struct option longopts[] = {
        { "help",    no_argument,       0, 'h' },
        { "verbose", no_argument,       0, 'v' },
        { "preview", optional_argument, 0, 'p' },
        { "run",     optional_argument, 0, 'r' },
        { 0,         0,                 0,  0 }
    };

    /* Options */
    int Verbose = 0;
    int Preview = 0;
    int Run     = 0;

    /* Parse options */
    int index = 0;
    int opt;

    while ((opt=getopt_long(argc, argv, shortopts, longopts, &index)) != -1)
    {
        switch (opt) {
        case 'h':
            usage();
            exit(0);
        case 'v':
            Verbose = 1;
            elm_set_verbose(Verbose);
            break;
        case 'p':
            Preview = 1;
            break;
        case 'r':
            Run = 1;
            break;
        default:
            printf("Unknown option specified '%c'.", opt);
            exit(1);
        }
    }

    /* Run login manager */
    if (Run) {
        ElmLoginManager *manager = elm_new_login_manager();
        manager->set_preview_mode(Preview);
        return manager->run();
    }
    return 0;
}

/* ************************************************************************** */
/* Print program usage */
void usage(void)
{
    printf("Usage: %s [options] <args>\n", PROG);
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
