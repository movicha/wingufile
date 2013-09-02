#ifndef SEAF_UTILS_H
#define SEAF_UTILS_H

struct _WingufileSession;


char *
wingufile_session_get_tmp_file_path (struct _WingufileSession *session,
                                   const char *basename,
                                   char path[]);

#ifdef WINGUFILE_SERVER
int
load_database_config (struct _WingufileSession *session);
#endif

#endif
