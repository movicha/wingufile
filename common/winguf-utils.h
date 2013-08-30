#ifndef SEAF_UTILS_H
#define SEAF_UTILS_H

struct _SeafileSession;


char *
wingufile_session_get_tmp_file_path (struct _SeafileSession *session,
                                   const char *basename,
                                   char path[]);

#ifdef WINGUFILE_SERVER
int
load_database_config (struct _SeafileSession *session);
#endif

#endif
