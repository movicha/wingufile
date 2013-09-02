#include "common.h"
#include "log.h"

#include <getopt.h>

#include <ccnet.h>

#include "wingufile-session.h"
#include "gc-core.h"
#include "verify.h"

#include "utils.h"

static char *config_dir = NULL;
static char *wingufile_dir = NULL;

CcnetClient *ccnet_client;
WingufileSession *winguf;

static const char *short_opts = "hvc:d:VDi";
static const struct option long_opts[] = {
    { "help", no_argument, NULL, 'h', },
    { "version", no_argument, NULL, 'v', },
    { "config-file", required_argument, NULL, 'c', },
    { "wingufdir", required_argument, NULL, 'd', },
    { "verify", no_argument, NULL, 'V' },
    { "dry-run", no_argument, NULL, 'D' },
    { "ignore-errors", no_argument, NULL, 'i' },
};

static void usage ()
{
    fprintf (stderr,
             "usage: wingufserv-gc [-c config_dir] [-d wingufile_dir]\n"
             "Additional options:\n"
             "-V, --verify: check for missing blocks\n");
}

static void
load_history_config ()
{
    int keep_history_days;
    GError *error = NULL;

    winguf->keep_history_days = -1;

    keep_history_days = g_key_file_get_integer (winguf->config,
                                                "history", "keep_days",
                                                &error);
    if (error == NULL)
        winguf->keep_history_days = keep_history_days;
}

#ifdef WIN32
/* Get the commandline arguments in unicode, then convert them to utf8  */
static char **
get_argv_utf8 (int *argc)
{
    int i = 0;
    char **argv = NULL;
    const wchar_t *cmdline = NULL;
    wchar_t **argv_w = NULL;

    cmdline = GetCommandLineW();
    argv_w = CommandLineToArgvW (cmdline, argc);
    if (!argv_w) {
        printf("failed to CommandLineToArgvW(), GLE=%lu\n", GetLastError());
        return NULL;
    }

    argv = (char **)malloc (sizeof(char*) * (*argc));
    for (i = 0; i < *argc; i++) {
        argv[i] = wchar_to_utf8 (argv_w[i]);
    }

    return argv;
}
#endif

int
main(int argc, char *argv[])
{
    int c;
    int verify = 0;
    int dry_run = 0;
    int ignore_errors = 0;

#ifdef WIN32
    argv = get_argv_utf8 (&argc);
#endif

    config_dir = DEFAULT_CONFIG_DIR;

    while ((c = getopt_long(argc, argv,
                short_opts, long_opts, NULL)) != EOF) {
        switch (c) {
        case 'h':
            usage();
            exit(0);
        case 'v':
            exit(-1);
            break;
        case 'c':
            config_dir = strdup(optarg);
            break;
        case 'd':
            wingufile_dir = strdup(optarg);
            break;
        case 'V':
            verify = 1;
            break;
        case 'D':
            dry_run = 1;
            break;
        case 'i':
            ignore_errors = 1;
            break;
        default:
            usage();
            exit(-1);
        }
    }

    g_type_init();

    if (wingufile_log_init ("-", "info", "debug") < 0) {
        winguf_warning ("Failed to init log.\n");
        exit (1);
    }

    ccnet_client = ccnet_client_new();
    if ((ccnet_client_load_confdir(ccnet_client, config_dir)) < 0) {
        winguf_warning ("Read config dir error\n");
        return -1;
    }

    if (wingufile_dir == NULL)
        wingufile_dir = g_build_filename (config_dir, "wingufile-data", NULL);
    
    winguf = wingufile_session_new(wingufile_dir, ccnet_client);
    if (!winguf) {
        winguf_warning ("Failed to create wingufile session.\n");
        exit (1);
    }

    load_history_config ();

    if (verify) {
        verify_repos ();
        return 0;
    }

    gc_core_run (dry_run, ignore_errors);

    return 0;
}
