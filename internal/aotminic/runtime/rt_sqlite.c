// rt_sqlite.c
// Minimal bridge implementation for SQLite.

#include "rt_sqlite.h"
#include <stdio.h>

int rt_sqlite_open(const char *path, rt_db *out) {
    if (!out) return 1;
    out->db = NULL;
    int rc = sqlite3_open(path, &out->db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "rt_sqlite_open failed: %s\n", out->db ? sqlite3_errmsg(out->db) : "(null)");
        return rc ? rc : 1;
    }
    return 0;
}

int rt_sqlite_exec(rt_db *db, const char *sql) {
    if (!db || !db->db || !sql) return 1;
    char *errmsg = NULL;
    int rc = sqlite3_exec(db->db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "rt_sqlite_exec error: %s\n", errmsg ? errmsg : "(null)");
        sqlite3_free(errmsg);
        return rc;
    }
    return 0;
}

int rt_sqlite_close(rt_db *db) {
    if (!db || !db->db) return 0;
    int rc = sqlite3_close(db->db);
    db->db = NULL;
    return rc == SQLITE_OK ? 0 : rc;
}