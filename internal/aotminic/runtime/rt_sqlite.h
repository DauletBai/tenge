// rt_sqlite.h
// Minimal bridge API for SQLite to be used by Tenge AOT-generated C.

#pragma once
#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    sqlite3 *db;
} rt_db;

int  rt_sqlite_open(const char *path, rt_db *out);
int  rt_sqlite_exec(rt_db *db, const char *sql);
int  rt_sqlite_close(rt_db *db);

#ifdef __cplusplus
}
#endif