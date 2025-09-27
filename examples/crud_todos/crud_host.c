#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static const char *DB_PATH = "todos.db";

static const char *DDL =
    "CREATE TABLE IF NOT EXISTS todos ("
    "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "  title TEXT NOT NULL,"
    "  priority INTEGER NOT NULL DEFAULT 0,"
    "  done INTEGER NOT NULL DEFAULT 0,"
    "  created_at TEXT NOT NULL DEFAULT (datetime('now'))"
    ");";

static void die(const char *msg, int rc) {
    fprintf(stderr, "error: %s (rc=%d)\n", msg, rc);
    exit(1);
}

static sqlite3* open_db(void) {
    sqlite3 *db = NULL;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) die("sqlite3_open", rc);
    return db;
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "sqlite error: %s\n", errmsg ? errmsg : "(null)");
        sqlite3_free(errmsg);
        exit(1);
    }
}

static void init_cmd(void) {
    sqlite3 *db = open_db();
    exec_sql(db, DDL);
    sqlite3_close(db);
    puts("initialized");
}

static void purge_cmd(void) {
    sqlite3 *db = open_db();
    exec_sql(db, "DROP TABLE IF EXISTS todos;");
    exec_sql(db, DDL);
    sqlite3_close(db);
    puts("purged");
}

static void add_cmd(const char *title, int priority) {
    sqlite3 *db = open_db();
    exec_sql(db, DDL);
    const char *SQL = "INSERT INTO todos(title, priority) VALUES(?, ?);";
    sqlite3_stmt *st = NULL;
    int rc = sqlite3_prepare_v2(db, SQL, -1, &st, NULL);
    if (rc != SQLITE_OK) die("prepare add", rc);
    sqlite3_bind_text(st, 1, title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 2, priority);
    rc = sqlite3_step(st);
    if (rc != SQLITE_DONE) die("step add", rc);
    sqlite3_finalize(st);
    sqlite3_close(db);
    puts("added");
}

static void list_cmd(void) {
    sqlite3 *db = open_db();
    exec_sql(db, DDL);
    const char *SQL =
        "SELECT id, title, priority, done, created_at "
        "FROM todos ORDER BY id ASC;";
    sqlite3_stmt *st = NULL;
    int rc = sqlite3_prepare_v2(db, SQL, -1, &st, NULL);
    if (rc != SQLITE_OK) die("prepare list", rc);

    printf("id   | title                                    | priority | done | created_at\n");
    printf("-----+------------------------------------------+----------+------+---------------------\n");
    while ((rc = sqlite3_step(st)) == SQLITE_ROW) {
        int id         = sqlite3_column_int(st, 0);
        const unsigned char *title = sqlite3_column_text(st, 1);
        int priority   = sqlite3_column_int(st, 2);
        int done       = sqlite3_column_int(st, 3);
        const unsigned char *created = sqlite3_column_text(st, 4);
        printf("%-4d | %-40s | %-8d | %-4d | %s\n",
               id,
               title ? (const char*)title : "",
               priority,
               done,
               created ? (const char*)created : "");
    }
    if (rc != SQLITE_DONE) die("step list", rc);
    sqlite3_finalize(st);
    sqlite3_close(db);
}

static void get_cmd(int id) {
    sqlite3 *db = open_db();
    exec_sql(db, DDL);
    const char *SQL =
        "SELECT id, title, priority, done, created_at FROM todos WHERE id=?;";
    sqlite3_stmt *st = NULL;
    int rc = sqlite3_prepare_v2(db, SQL, -1, &st, NULL);
    if (rc != SQLITE_OK) die("prepare get", rc);
    sqlite3_bind_int(st, 1, id);
    rc = sqlite3_step(st);
    if (rc == SQLITE_ROW) {
        printf("id=%d\n", sqlite3_column_int(st, 0));
        printf("title=%s\n", sqlite3_column_text(st, 1));
        printf("priority=%d\n", sqlite3_column_int(st, 2));
        printf("done=%d\n", sqlite3_column_int(st, 3));
        printf("created_at=%s\n", sqlite3_column_text(st, 4));
    } else {
        printf("not found\n");
    }
    sqlite3_finalize(st);
    sqlite3_close(db);
}

static void done_cmd(int id) {
    sqlite3 *db = open_db();
    exec_sql(db, DDL);
    const char *SQL = "UPDATE todos SET done=1 WHERE id=?;";
    sqlite3_stmt *st = NULL;
    int rc = sqlite3_prepare_v2(db, SQL, -1, &st, NULL);
    if (rc != SQLITE_OK) die("prepare done", rc);
    sqlite3_bind_int(st, 1, id);
    rc = sqlite3_step(st);
    if (rc != SQLITE_DONE) die("step done", rc);
    sqlite3_finalize(st);
    sqlite3_close(db);
    puts("done");
}

static void rm_cmd(int id) {
    sqlite3 *db = open_db();
    exec_sql(db, DDL);
    const char *SQL = "DELETE FROM todos WHERE id=?;";
    sqlite3_stmt *st = NULL;
    int rc = sqlite3_prepare_v2(db, SQL, -1, &st, NULL);
    if (rc != SQLITE_OK) die("prepare rm", rc);
    sqlite3_bind_int(st, 1, id);
    rc = sqlite3_step(st);
    if (rc != SQLITE_DONE) die("step rm", rc);
    sqlite3_finalize(st);
    sqlite3_close(db);
    puts("removed");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "usage:\n"
            "  %s init\n"
            "  %s purge\n"
            "  %s add <title> <priority>\n"
            "  %s list\n"
            "  %s get <id>\n"
            "  %s done <id>\n"
            "  %s rm <id>\n", argv[0],argv[0],argv[0],argv[0],argv[0],argv[0],argv[0]);
        return 1;
    }
    const char *cmd = argv[1];
    if (strcmp(cmd, "init") == 0) {
        init_cmd();
    } else if (strcmp(cmd, "purge") == 0) {
        purge_cmd();
    } else if (strcmp(cmd, "add") == 0) {
        if (argc < 4) die("add requires <title> <priority>", -1);
        const char *title = argv[2];
        int priority = atoi(argv[3]);
        add_cmd(title, priority);
    } else if (strcmp(cmd, "list") == 0) {
        list_cmd();
    } else if (strcmp(cmd, "get") == 0) {
        if (argc < 3) die("get requires <id>", -1);
        get_cmd(atoi(argv[2]));
    } else if (strcmp(cmd, "done") == 0) {
        if (argc < 3) die("done requires <id>", -1);
        done_cmd(atoi(argv[2]));
    } else if (strcmp(cmd, "rm") == 0) {
        if (argc < 3) die("rm requires <id>", -1);
        rm_cmd(atoi(argv[2]));
    } else {
        die("unknown command", -1);
    }
    return 0;
}