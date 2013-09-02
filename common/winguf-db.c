
#include "common.h"

#include <zdb.h>
#include "winguf-db.h"

#ifdef WIN32
#include <windows.h>
#define sleep(n) Sleep(1000 * (n))
#endif

#define MAX_GET_CONNECTION_RETRIES 3

struct SeafDB {
    int type;
    ConnectionPool_T pool;
};

struct SeafDBRow {
    ResultSet_T res;
};

struct SeafDBTrans {
    Connection_T conn;
};

SeafDB *
winguf_db_new_mysql (const char *host, 
                   const char *user, 
                   const char *passwd,
                   const char *db_name,
                   const char *unix_socket)
{
    SeafDB *db;
    GString *url;
    URL_T zdb_url;

    db = g_new0 (SeafDB, 1);
    if (!db) {
        g_warning ("Failed to alloc db structure.\n");
        return NULL;
    }

    url = g_string_new ("");
    g_string_append_printf (url, "mysql://%s:%s@%s/", user, passwd, host);
    if (db_name)
        g_string_append (url, db_name);
    if (unix_socket)
        g_string_append_printf (url, "?unix-socket=%s", unix_socket);

    zdb_url = URL_new (url->str);
    db->pool = ConnectionPool_new (zdb_url);
    if (!db->pool) {
        g_warning ("Failed to create db connection pool.\n");
        g_string_free (url, TRUE);
        g_free (db);
        return NULL;
    }

    ConnectionPool_start (db->pool);
    db->type = SEAF_DB_TYPE_MYSQL;

    return db;
}

SeafDB *
winguf_db_new_pgsql (const char *host,
                   const char *user,
                   const char *passwd,
                   const char *db_name,
                   const char *unix_socket)
{
    SeafDB *db;
    GString *url;
    URL_T zdb_url;

    db = g_new0(SeafDB, 1);
    if (!db) {
        g_warning ("Failed to alloc db structre.\n");
        return NULL;
    }

    url = g_string_new ("");
    g_string_append_printf (url, "postgresql://%s:%s@%s/", user, passwd, host);
    if (db_name)
        g_string_append (url, db_name);
    if (unix_socket)
        g_string_append_printf (url, "?unix-socket=%s", unix_socket);

    zdb_url = URL_new (url->str);
    db->pool = ConnectionPool_new (zdb_url);
    if (!db->pool) {
        g_warning ("Failed to create db connection pool.\n");
        g_string_free(url, TRUE);
        g_free (db);
        return NULL;
    }

    ConnectionPool_start (db->pool);
    db->type = SEAF_DB_TYPE_PGSQL;

    return db;
}

SeafDB *
winguf_db_new_sqlite (const char *db_path)
{
    SeafDB *db;
    GString *url;
    URL_T zdb_url;

    db = g_new0 (SeafDB, 1);
    if (!db) {
        g_warning ("Failed to alloc db structure.\n");
        return NULL;
    }

    url = g_string_new ("");
    g_string_append_printf (url, "sqlite://%s", db_path);

    zdb_url = URL_new (url->str);
    db->pool = ConnectionPool_new (zdb_url);
    if (!db->pool) {
        g_warning ("Failed to create db connection pool.\n");
        g_string_free (url, TRUE);
        g_free (db);
        return NULL;
    }

    ConnectionPool_start (db->pool);
    db->type = SEAF_DB_TYPE_SQLITE;

    return db;
}

void
winguf_db_free (SeafDB *db)
{
    ConnectionPool_stop (db->pool);
    ConnectionPool_free (&db->pool);
    g_free (db);
}

int
winguf_db_type (SeafDB *db)
{
    return db->type;
}

static Connection_T
get_db_connection (SeafDB *db)
{
    Connection_T conn;
    int retries = 0;

    conn = ConnectionPool_getConnection (db->pool);
    /* If max_connections of the pool has been reached, retry 3 times
     * and then return NULL.
     */
    while (!conn) {
        if (retries++ == MAX_GET_CONNECTION_RETRIES) {
            g_warning ("Too many concurrent connections. "
                       "Failed to create new connection.\n");
            goto out;
        }
        sleep (1);
        conn = ConnectionPool_getConnection (db->pool);
    }

    if (!conn)
        g_warning ("Failed to create new connection.\n");

out:
    return conn;
}

int
winguf_db_query (SeafDB *db, const char *sql)
{
    Connection_T conn = get_db_connection (db);
    if (!conn)
        return -1;

    /* Handle zdb "exception"s. */
    TRY
        Connection_execute (conn, "%s", sql);
        Connection_close (conn);
        RETURN (0);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return -1;
    END_TRY;

    /* Should not be reached. */
    return 0;
}

gboolean
winguf_db_check_for_existence (SeafDB *db, const char *sql, gboolean *db_err)
{
    Connection_T conn;
    ResultSet_T result;
    gboolean ret = TRUE;

    *db_err = FALSE;

    conn = get_db_connection (db);
    if (!conn) {
        *db_err = TRUE;
        return FALSE;
    }

    TRY
        result = Connection_executeQuery (conn, "%s", sql);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        *db_err = TRUE;
        return FALSE;
    END_TRY;

    TRY
        if (!ResultSet_next (result))
            ret = FALSE;
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        *db_err = TRUE;
        return FALSE;
    END_TRY;

    Connection_close (conn);

    return ret;
}

int
winguf_db_foreach_selected_row (SeafDB *db, const char *sql, 
                              SeafDBRowFunc callback, void *data)
{
    Connection_T conn;
    ResultSet_T result;
    SeafDBRow winguf_row;
    int n_rows = 0;

    conn = get_db_connection (db);
    if (!conn)
        return -1;

    TRY
        result = Connection_executeQuery (conn, "%s", sql);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return -1;
    END_TRY;

    winguf_row.res = result;
    TRY
        while (ResultSet_next (result)) {
            n_rows++;
            if (!callback (&winguf_row, data))
                break;
        }
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return -1;
    END_TRY;

    Connection_close (conn);
    return n_rows;
}

const char *
winguf_db_row_get_column_text (SeafDBRow *row, guint32 idx)
{
    g_assert (idx < ResultSet_getColumnCount(row->res));

    return ResultSet_getString (row->res, idx+1);
}

int
winguf_db_row_get_column_int (SeafDBRow *row, guint32 idx)
{
    g_assert (idx < ResultSet_getColumnCount(row->res));

    return ResultSet_getInt (row->res, idx+1);
}

gint64
winguf_db_row_get_column_int64 (SeafDBRow *row, guint32 idx)
{
    g_assert (idx < ResultSet_getColumnCount(row->res));

    return ResultSet_getLLong (row->res, idx+1);
}

int
winguf_db_get_int (SeafDB *db, const char *sql)
{
    int ret = -1;
    Connection_T conn;
    ResultSet_T result;
    SeafDBRow winguf_row;

    conn = get_db_connection (db);
    if (!conn)
        return -1;

    TRY
        result = Connection_executeQuery (conn, "%s", sql);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return -1;
    END_TRY;

    winguf_row.res = result;

    TRY
        if (ResultSet_next (result))
            ret = winguf_db_row_get_column_int (&winguf_row, 0);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return -1;
    END_TRY;

    Connection_close (conn);
    return ret;
}

gint64
winguf_db_get_int64 (SeafDB *db, const char *sql)
{
    gint64 ret = -1;
    Connection_T conn;
    ResultSet_T result;
    SeafDBRow winguf_row;

    conn = get_db_connection (db);
    if (!conn)
        return -1;

    TRY
        result = Connection_executeQuery (conn, "%s", sql);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return -1;
    END_TRY;

    winguf_row.res = result;

    TRY
        if (ResultSet_next (result))
            ret = winguf_db_row_get_column_int64 (&winguf_row, 0);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return -1;
    END_TRY;

    Connection_close (conn);
    return ret;
}

char *
winguf_db_get_string (SeafDB *db, const char *sql)
{
    char *ret = NULL;
    const char *s;
    Connection_T conn;
    ResultSet_T result;
    SeafDBRow winguf_row;

    conn = get_db_connection (db);
    if (!conn)
        return NULL;

    TRY
        result = Connection_executeQuery (conn, "%s", sql);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return NULL;
    END_TRY;

    winguf_row.res = result;
    
    TRY
        if (ResultSet_next (result)) {
            s = winguf_db_row_get_column_text (&winguf_row, 0);
            ret = g_strdup(s);
        }
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        Connection_close (conn);
        return NULL;
    END_TRY;

    Connection_close (conn);
    return ret;
}

SeafDBTrans *
winguf_db_begin_transaction (SeafDB *db)
{
    Connection_T conn;
    SeafDBTrans *trans;

    trans = g_new0 (SeafDBTrans, 1);
    if (!trans)
        return NULL;

    conn = get_db_connection (db);
    if (!conn) {
        g_free (trans);
        return NULL;
    }

    trans->conn = conn;
    TRY
        Connection_beginTransaction (trans->conn);
    CATCH (SQLException)
        g_warning ("Start transaction failed: %s.\n", Exception_frame.message);
        Connection_close (trans->conn);
        g_free (trans);
        return NULL;
    END_TRY;

    return trans;
}

void
winguf_db_trans_close (SeafDBTrans *trans)
{
    Connection_close (trans->conn);
    g_free (trans);
}

int
winguf_db_commit (SeafDBTrans *trans)
{
    Connection_T conn = trans->conn;

    TRY
        Connection_commit (conn);
    CATCH (SQLException)
        g_warning ("Commit failed: %s.\n", Exception_frame.message);
        return -1;
    END_TRY;

    return 0;
}

int
winguf_db_rollback (SeafDBTrans *trans)
{
    Connection_T conn = trans->conn;

    TRY
        Connection_rollback (conn);
    CATCH (SQLException)
        g_warning ("Rollback failed: %s.\n", Exception_frame.message);
        return -1;
    END_TRY;

    return 0;
}

int
winguf_db_trans_query (SeafDBTrans *trans, const char *sql)
{
    /* Handle zdb "exception"s. */
    TRY
        Connection_execute (trans->conn, "%s", sql);
        RETURN (0);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        return -1;
    END_TRY;

    /* Should not be reached. */
    return 0;
}

gboolean
winguf_db_trans_check_for_existence (SeafDBTrans *trans,
                                   const char *sql,
                                   gboolean *db_err)
{
    ResultSet_T result;
    gboolean ret = TRUE;

    *db_err = FALSE;

    TRY
        result = Connection_executeQuery (trans->conn, "%s", sql);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        *db_err = TRUE;
        return FALSE;
    END_TRY;

    TRY
        if (!ResultSet_next (result))
            ret = FALSE;
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        *db_err = TRUE;
        return FALSE;
    END_TRY;

    return ret;
}

int
winguf_db_trans_foreach_selected_row (SeafDBTrans *trans, const char *sql, 
                              SeafDBRowFunc callback, void *data)
{
    ResultSet_T result;
    SeafDBRow winguf_row;
    int n_rows = 0;

    TRY
        result = Connection_executeQuery (trans->conn, "%s", sql);
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        return -1;
    END_TRY;

    winguf_row.res = result;
    TRY
    while (ResultSet_next (result)) {
        n_rows++;
        if (!callback (&winguf_row, data))
            break;
    }
    CATCH (SQLException)
        g_warning ("Error exec query %s: %s.\n", sql, Exception_frame.message);
        return -1;
    END_TRY;

    return n_rows;
}

char *
winguf_db_escape_string (SeafDB *db, const char *from)
{
    const char *p = from;
    char *to, *q;

    to = g_malloc0 (2*strlen(from)+1);
    q = to;

    while (*p != '\0') {
        if (*p == '\'' || *p == '\\' || *p == '"') {
            *q = *p;
            *(++q) = *p;
        } else
            *q = *p;
        ++p;
        ++q;
    }

    return to;
}

gboolean
pgsql_index_exists (SeafDB *db, const char *index_name)
{
    char sql[256];
    gboolean db_err = FALSE;

    snprintf (sql, sizeof(sql),
              "SELECT 1 FROM pg_class WHERE relname='%s'",
              index_name);
    return winguf_db_check_for_existence (db, sql, &db_err);
}
