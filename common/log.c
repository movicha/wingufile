/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include <stdio.h>
#include <glib/gstdio.h>

#include "log.h"
#include "utils.h"

/* message with greater log levels will be ignored */
static int ccnet_log_level;
static int wingufile_log_level;
static FILE *logfp;

static void 
wingufile_log (const gchar *log_domain, GLogLevelFlags log_level,
             const gchar *message,    gpointer user_data)
{
    time_t t;
    struct tm *tm;
    char buf[1024];
    int len;

    if (log_level > wingufile_log_level)
        return;

    t = time(NULL);
    tm = localtime(&t);
    len = strftime (buf, 1024, "[%x %X] ", tm);
    g_assert(len < 1024);
    fputs (buf, logfp);
    fputs (message, logfp);
    fflush (logfp);
}

static void 
ccnet_log (const gchar *log_domain, GLogLevelFlags log_level,
             const gchar *message,    gpointer user_data)
{
    time_t t;
    struct tm *tm;
    char buf[1024];
    int len;

    if (log_level > ccnet_log_level)
        return;

    t = time(NULL);
    tm = localtime(&t);
    len = strftime (buf, 1024, "[%x %X] ", tm);
    g_assert(len < 1024);
    fputs (buf, logfp);
    fputs (message, logfp);
    fflush (logfp);
}

static int
get_debug_level(const char *str, int default_level)
{
    if (strcmp(str, "debug") == 0)
        return G_LOG_LEVEL_DEBUG;
    if (strcmp(str, "info") == 0)
        return G_LOG_LEVEL_INFO;
    if (strcmp(str, "warning") == 0)
        return G_LOG_LEVEL_WARNING;
    return default_level;
}

int
wingufile_log_init (const char *logfile, const char *ccnet_debug_level_str,
                  const char *wingufile_debug_level_str)
{
    g_log_set_handler (NULL, G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
                       | G_LOG_FLAG_RECURSION, wingufile_log, NULL);
    g_log_set_handler ("Ccnet", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
                       | G_LOG_FLAG_RECURSION, ccnet_log, NULL);

    /* record all log message */
    ccnet_log_level = get_debug_level(ccnet_debug_level_str, G_LOG_LEVEL_INFO);
    wingufile_log_level = get_debug_level(wingufile_debug_level_str, G_LOG_LEVEL_DEBUG);

    if (strcmp(logfile, "-") == 0)
        logfp = stdout;
    else {
        logfile = ccnet_expand_path(logfile);
        if ((logfp = g_fopen (logfile, "a+")) == NULL) {
            return -1;
        }
    }

    return 0;
}


static WingufileDebugFlags debug_flags = 0;

static GDebugKey debug_keys[] = {
  { "Transfer", WINGUFILE_DEBUG_TRANSFER },
  { "Sync", WINGUFILE_DEBUG_SYNC },
  { "Watch", WINGUFILE_DEBUG_WATCH },
  { "Http", WINGUFILE_DEBUG_HTTP },
  { "Merge", WINGUFILE_DEBUG_MERGE },
  { "Other", WINGUFILE_DEBUG_OTHER },
};

gboolean
wingufile_debug_flag_is_set (WingufileDebugFlags flag)
{
    return (debug_flags & flag) != 0;
}

void
wingufile_debug_set_flags (WingufileDebugFlags flags)
{
    g_message ("Set debug flags %#x\n", flags);
    debug_flags |= flags;
}

void
wingufile_debug_set_flags_string (const gchar *flags_string)
{
    guint nkeys = G_N_ELEMENTS (debug_keys);

    if (flags_string)
        wingufile_debug_set_flags (
            g_parse_debug_string (flags_string, debug_keys, nkeys));
}

void
wingufile_debug_impl (WingufileDebugFlags flag, const gchar *format, ...)
{
    if (flag & debug_flags) {
        va_list args;
        va_start (args, format);
        g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, format, args);
        va_end (args);
    }
}
