#ifndef SEAF_DB_H
#define SEAF_DB_H

enum {
    SEAF_DB_TYPE_SQLITE,
    SEAF_DB_TYPE_MYSQL,
    SEAF_DB_TYPE_PGSQL,
};

typedef struct SeafDB SeafDB;
typedef struct SeafDBRow SeafDBRow;
typedef struct SeafDBTrans SeafDBTrans;

typedef gboolean (*SeafDBRowFunc) (SeafDBRow *, void *);

SeafDB *
winguf_db_new_mysql (const char *host, 
                   const char *user, 
                   const char *passwd,
                   const char *db,
                   const char *unix_socket);

SeafDB *
winguf_db_new_pgsql (const char *host,
                   const char *user,
                   const char *passwd,
                   const char *db_name,
                   const char *unix_socket);

SeafDB *
winguf_db_new_sqlite (const char *db_path);

void
winguf_db_free (SeafDB *db);

int
winguf_db_type (SeafDB *db);

int
winguf_db_query (SeafDB *db, const char *sql);

gboolean
winguf_db_check_for_existence (SeafDB *db, const char *sql, gboolean *db_err);

int
winguf_db_foreach_selected_row (SeafDB *db, const char *sql, 
                              SeafDBRowFunc callback, void *data);

const char *
winguf_db_row_get_column_text (SeafDBRow *row, guint32 idx);

int
winguf_db_row_get_column_int (SeafDBRow *row, guint32 idx);

gint64
winguf_db_row_get_column_int64 (SeafDBRow *row, guint32 idx);

int
winguf_db_get_int (SeafDB *db, const char *sql);

gint64
winguf_db_get_int64 (SeafDB *db, const char *sql);

char *
winguf_db_get_string (SeafDB *db, const char *sql);

/* Transaction related */

SeafDBTrans *
winguf_db_begin_transaction (SeafDB *db);

void
winguf_db_trans_close (SeafDBTrans *trans);

int
winguf_db_commit (SeafDBTrans *trans);

int
winguf_db_rollback (SeafDBTrans *trans);

int
winguf_db_trans_query (SeafDBTrans *trans, const char *sql);

gboolean
winguf_db_trans_check_for_existence (SeafDBTrans *trans,
                                   const char *sql,
                                   gboolean *db_err);

int
winguf_db_trans_foreach_selected_row (SeafDBTrans *trans, const char *sql,
                                    SeafDBRowFunc callback, void *data);

/* Escape a string contant by doubling '\" characters.
 */
char *
winguf_db_escape_string (SeafDB *db, const char *from);

gboolean
pgsql_index_exists (SeafDB *db, const char *index_name);

#endif
