/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"
#include "log.h"

#include <getopt.h>

#include <event.h>
#include <evhtp.h>

#include <ccnet.h>

#include "wingufile-session.h"
#include "httpserver.h"
#include "access-file.h"
#include "upload-file.h"

#include "utils.h"

#define DEFAULT_BIND_PORT  8082
#define DEFAULT_MAX_UPLOAD_SIZE 100 * ((gint64)1 << 20) /* 100MB */
#define DEFAULT_MAX_DOWNLOAD_DIR_SIZE 100 * ((gint64)1 << 20) /* 100MB */

static char *config_dir = NULL;
static char *wingufile_dir = NULL;
static char *bind_addr = "0.0.0.0";
static gboolean use_https = FALSE;
static uint16_t bind_port = 0;
static int num_threads = 10;
static char *pemfile = NULL;
static char *privkey = NULL;

static char *pidfile = NULL;

CcnetClient *ccnet_client;
WingufileSession *winguf;

static const char *short_opts = "hvfc:d:t:l:g:G:D:k:P:";
static const struct option long_opts[] = {
    { "help", no_argument, NULL, 'h', },
    { "version", no_argument, NULL, 'v', },
    { "foreground", no_argument, NULL, 'f', },
    { "ccnet-config-dir", required_argument, NULL, 'c', },
    { "wingufdir", required_argument, NULL, 'd', },
    { "threads", required_argument, NULL, 't', },
    { "log", required_argument, NULL, 'l' },
    { "ccnet-debug-level", required_argument, NULL, 'g' },
    { "http-debug-level", required_argument, NULL, 'G' },
    { "debug", required_argument, NULL, 'D' },
    { "temp-file-dir", required_argument, NULL, 'k' },
    { "pidfile", required_argument, NULL, 'P' },
};

static void usage ()
{
    fprintf (stderr,
             "usage: httpserver [-c config_dir] [-d wingufile_dir] \n");
}

static void
default_cb(evhtp_request_t *req, void *arg)
{
    /* Return empty page. */
    evhtp_send_reply (req, EVHTP_RES_OK);
}

static void
remove_pidfile (const char *pidfile)
{
    if (pidfile) {
        g_unlink (pidfile);
    }
}

static int
write_pidfile (const char *pidfile_path)
{
    if (!pidfile_path)
        return -1;

    pid_t pid = getpid();

    FILE *pidfile = g_fopen(pidfile_path, "w");
    if (!pidfile) {
        winguf_warning ("Failed to fopen() pidfile %s: %s\n",
                      pidfile_path, strerror(errno));
        return -1;
    }

    char buf[32];
    snprintf (buf, sizeof(buf), "%d\n", pid);
    if (fputs(buf, pidfile) < 0) {
        winguf_warning ("Failed to write pidfile %s: %s\n",
                      pidfile_path, strerror(errno));
        fclose (pidfile);
        return -1;
    }

    fflush (pidfile);
    fclose (pidfile);
    return 0;
}

static void
on_httpserver_exit(void)
{
    if (pidfile)
        remove_pidfile (pidfile);
}

static void
load_httpserver_config (WingufileSession *session)
{
    GError *error = NULL;
    int port = 0;
    int max_upload_size_mb;
    int max_download_dir_size_mb;

    port = g_key_file_get_integer (session->config, "httpserver", "port", &error);
    if (!error) {
        bind_port = port;
    } else {
        if (error->code != G_KEY_FILE_ERROR_KEY_NOT_FOUND &&
            error->code != G_KEY_FILE_ERROR_GROUP_NOT_FOUND) {
            winguf_warning ("[conf] Error: failed to read the value of 'port'\n");
            exit (1);
        }

        bind_port = DEFAULT_BIND_PORT;
        g_clear_error (&error);
    }

    use_https = g_key_file_get_boolean (session->config,
                                        "httpserver", "https",
                                        &error);
    if (error) {
        if (error->code != G_KEY_FILE_ERROR_KEY_NOT_FOUND &&
            error->code != G_KEY_FILE_ERROR_GROUP_NOT_FOUND) {
            winguf_warning ("[conf] Error: failed to read the value of 'https'\n");
            exit (1);
        }

        /* There is no <https> field in wingufile.conf, so we use http. */
        g_clear_error (&error);

    } else if (use_https) {
        /* read https config */
        pemfile = g_key_file_get_string (session->config,
                                         "httpserver", "pemfile",
                                         &error);
        if (error) {
            winguf_warning ("[conf] Error: https is true, "
                          "but the value of pemfile is unknown\n");
            exit (1);
        }

        privkey = g_key_file_get_string (session->config,
                                         "httpserver", "privkey",
                                         &error);
        if (error) {
            winguf_warning ("[conf] Error: https is true, "
                          "but the value of privkey is unknown\n");
            exit (1);
        }
    }

    max_upload_size_mb = g_key_file_get_integer (session->config,
                                                 "httpserver",
                                                 "max_upload_size",
                                                 &error);
    if (error) {
        session->max_upload_size = DEFAULT_MAX_UPLOAD_SIZE;
        g_clear_error (&error);
    } else {
        if (max_upload_size_mb <= 0)
            session->max_upload_size = DEFAULT_MAX_UPLOAD_SIZE;
        else
            session->max_upload_size = max_upload_size_mb * ((gint64)1 << 20);
    }

    max_download_dir_size_mb = g_key_file_get_integer (session->config,
                                                 "httpserver",
                                                 "max_download_dir_size",
                                                 &error);
    if (error) {
        session->max_download_dir_size = DEFAULT_MAX_DOWNLOAD_DIR_SIZE;
        g_clear_error (&error);
    } else {
        if (max_download_dir_size_mb <= 0)
            session->max_download_dir_size = DEFAULT_MAX_DOWNLOAD_DIR_SIZE;
        else
            session->max_download_dir_size = max_download_dir_size_mb * ((gint64)1 << 20);
    }
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
    evbase_t *evbase = NULL;
    evhtp_t *htp = NULL;
    int daemon_mode = 1;
    int c;
    char *logfile = NULL;
    char *ccnet_debug_level_str = "info";
    char *http_debug_level_str = "debug";
    const char *debug_str = NULL;
    char *temp_file_dir = NULL;

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
        case 't':
            num_threads = atoi(optarg);
            break;
        case 'f':
            daemon_mode = 0;
            break;
        case 'l':
            logfile = g_strdup(optarg);
            break;
        case 'g':
            ccnet_debug_level_str = optarg;
            break;
        case 'G':
            http_debug_level_str = optarg;
            break;
        case 'D':
            debug_str = optarg;
            break;
        case 'k':
            temp_file_dir = optarg;
            break;
        case 'P':
            pidfile = optarg;
            break;
        default:
            usage();
            exit(-1);
        }
    }

#ifndef WIN32
    if (daemon_mode)
        daemon(1, 0);
#else
    WSADATA     wsadata;
    WSAStartup(0x0101, &wsadata);
#endif

    g_type_init();

    if (!debug_str)
        debug_str = g_getenv("WINGUFILE_DEBUG");
    wingufile_debug_set_flags_string (debug_str);

    if (wingufile_dir == NULL)
        wingufile_dir = g_build_filename (config_dir, "wingufile-data", NULL);
    if (logfile == NULL)
        logfile = g_build_filename (wingufile_dir, "http.log", NULL);

    if (wingufile_log_init (logfile, ccnet_debug_level_str,
                          http_debug_level_str) < 0) {
        g_warning ("Failed to init log.\n");
        exit (1);
    }

    ccnet_client = ccnet_client_new();
    if ((ccnet_client_load_confdir(ccnet_client, config_dir)) < 0) {
        g_warning ("Read config dir error\n");
        return -1;
    }

    winguf = wingufile_session_new (wingufile_dir, ccnet_client);
    if (!winguf) {
        g_warning ("Failed to create wingufile session.\n");
        exit (1);
    }
    if (wingufile_session_init(winguf) < 0)
        exit (1);

    if (temp_file_dir == NULL)
        winguf->http_temp_dir = g_build_filename (winguf->winguf_dir, "httptemp", NULL);
    else
        winguf->http_temp_dir = g_strdup(temp_file_dir);

    winguf->client_pool = ccnet_client_pool_new (config_dir);

    load_httpserver_config (winguf);
    if (use_https) {
        winguf_message ("port = %d, https = true, pemfile = %s, privkey = %s\n",
                      bind_port, pemfile, privkey);
    } else {
        winguf_message ("port = %d, https = false\n", bind_port);
    }

    evbase = event_base_new();
    htp = evhtp_new(evbase, NULL);

    if (pemfile != NULL) {
        evhtp_ssl_cfg_t scfg;

        memset (&scfg, 0, sizeof(scfg));

        scfg.pemfile        = pemfile;
        scfg.privfile       = privkey;
        scfg.scache_type    = evhtp_ssl_scache_type_internal;
        scfg.scache_timeout = 5000;

        evhtp_ssl_init (htp, &scfg);
    }

    if (access_file_init (htp) < 0)
        exit (1);

    if (upload_file_init (htp) < 0)
        exit (1);

    evhtp_set_gencb(htp, default_cb, NULL);

    evhtp_use_threads(htp, NULL, num_threads, NULL);

    if (evhtp_bind_socket(htp, bind_addr, bind_port, 128) < 0) {
        g_warning ("Could not bind socket: %s\n", strerror(errno));
        exit(-1);
    }

    if (pidfile) {
        if (write_pidfile (pidfile) < 0) {
            winguf_message ("Failed to write pidfile\n");
            return -1;
        }
    }
    
    atexit (on_httpserver_exit);

    event_base_loop(evbase, 0);

    return 0;
}
