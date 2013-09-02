#ifndef LOG_H
#define LOG_H

#define WINGUFILE_DOMAIN g_quark_from_string("wingufile")

#ifndef winguf_warning
#define winguf_warning(fmt, ...) g_warning("%s(%d): " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#ifndef winguf_message
#define winguf_message(fmt, ...) g_message("%s(%d): " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#endif


int wingufile_log_init (const char *logfile, const char *ccnet_debug_level_str,
                      const char *wingufile_debug_level_str);

void
wingufile_debug_set_flags_string (const gchar *flags_string);

typedef enum
{
    WINGUFILE_DEBUG_TRANSFER = 1 << 1,
    WINGUFILE_DEBUG_SYNC = 1 << 2,
    WINGUFILE_DEBUG_WATCH = 1 << 3, /* wt-monitor */
    WINGUFILE_DEBUG_HTTP = 1 << 4,  /* http server */
    WINGUFILE_DEBUG_MERGE = 1 << 5,
    WINGUFILE_DEBUG_OTHER = 1 << 6,
} WingufileDebugFlags;

void wingufile_debug_impl (WingufileDebugFlags flag, const gchar *format, ...);

#ifdef DEBUG_FLAG

#undef winguf_debug
#define winguf_debug(fmt, ...)  \
    wingufile_debug_impl (DEBUG_FLAG, "%.10s(%d): " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif  /* DEBUG_FLAG */

#endif
