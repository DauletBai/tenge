#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const char *DB_PATH = "todos.db";
static const char *DDL =
    "CREATE TABLE IF NOT EXISTS todos ("
    "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "  title TEXT NOT NULL,"
    "  priority INTEGER NOT NULL DEFAULT 0,"
    "  done INTEGER NOT NULL DEFAULT 0,"
    "  created_at TEXT NOT NULL DEFAULT (datetime('now'))"
    ");";

static void die(const char *m) { perror(m); exit(1); }
static void http_400(int fd){ dprintf(fd,"HTTP/1.1 400 Bad Request\r\nContent-Length:0\r\n\r\n"); }
static void http_404(int fd){ dprintf(fd,"HTTP/1.1 404 Not Found\r\nContent-Length:0\r\n\r\n"); }
static void http_500(int fd){ dprintf(fd,"HTTP/1.1 500 Internal Server Error\r\nContent-Length:0\r\n\r\n"); }

static sqlite3* open_db(void){
    sqlite3 *db=NULL; if(sqlite3_open(DB_PATH,&db)!=SQLITE_OK){ return NULL; }
    char *errmsg=NULL;
    if(sqlite3_exec(db,DDL,NULL,NULL,&errmsg)!=SQLITE_OK){
        if(errmsg){ fprintf(stderr,"sqlite: %s\n",errmsg); sqlite3_free(errmsg); }
        sqlite3_close(db); return NULL;
    }
    return db;
}

static char from_hex(char c){ if(c>='0'&&c<='9') return c-'0'; c=tolower((unsigned char)c);
    if(c>='a'&&c<='f') return c-'a'+10; return 0; }

static void url_decode(char *s){
    char *o=s;
    for(; *s; s++,o++){
        if(*s=='%' && isxdigit((unsigned char)s[1]) && isxdigit((unsigned char)s[2])){
            *o = (from_hex(s[1])<<4) | from_hex(s[2]); s+=2;
        } else if(*s=='+') { *o=' '; } else { *o=*s; }
    }
    *o='\0';
}

static const char* qget(const char *qs, const char *key, char *buf, size_t bufsz){
    if(!qs) return NULL;
    size_t klen = strlen(key);
    const char *p = qs;
    while(p && *p){
        const char *eq = strchr(p,'=');
        const char *amp = strchr(p,'&');
        if(!amp) amp = p + strlen(p);
        if(eq && (size_t)(eq-p)==klen && strncmp(p,key,klen)==0){
            size_t vlen = (size_t)(amp - (eq+1));
            if(vlen >= bufsz) vlen = bufsz-1;
            strncpy(buf, eq+1, vlen); buf[vlen]='\0';
            url_decode(buf);
            return buf;
        }
        p = (*amp) ? amp+1 : NULL;
    }
    return NULL;
}

static void send_json(int fd, const char *json){
    dprintf(fd,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Cache-Control: no-store\r\n"
        "Content-Length: %zu\r\n\r\n%s",
        strlen(json), json);
}

static void handle_list(int fd){
    sqlite3 *db = open_db(); if(!db){ http_500(fd); return; }
    const char *SQL = "SELECT id,title,priority,done,created_at FROM todos ORDER BY id ASC;";
    sqlite3_stmt *st=NULL; if(sqlite3_prepare_v2(db,SQL,-1,&st,NULL)!=SQLITE_OK){ sqlite3_close(db); http_500(fd); return; }
    char *out = NULL; size_t cap=1024,len=0; out = malloc(cap); if(!out){ sqlite3_finalize(st); sqlite3_close(db); http_500(fd); return; }
    len += snprintf(out+len,cap-len,"[");
    int first=1;
    while(sqlite3_step(st)==SQLITE_ROW){
        if(!first){ len += snprintf(out+len,cap-len,","); } first=0;
        const char *title=(const char*)sqlite3_column_text(st,1);
        int id=sqlite3_column_int(st,0), pr=sqlite3_column_int(st,2), dn=sqlite3_column_int(st,3);
        const char *ts=(const char*)sqlite3_column_text(st,4);
        char obj[1024];
        int n = snprintf(obj,sizeof(obj),
            "{\"id\":%d,\"title\":\"%s\",\"priority\":%d,\"done\":%d,\"created_at\":\"%s\"}",
            id, title?title:"", pr, dn, ts?ts:"");
        while(len + (size_t)n + 2 > cap){ cap*=2; out = realloc(out,cap); if(!out){ sqlite3_finalize(st); sqlite3_close(db); http_500(fd); return; } }
        memcpy(out+len,obj,(size_t)n); len += (size_t)n;
    }
    sqlite3_finalize(st); sqlite3_close(db);
    if(len+2>cap){ cap+=2; out=realloc(out,cap); if(!out){ http_500(fd); return; } }
    out[len++]=']'; out[len]='\0';
    send_json(fd,out);
    free(out);
}

static void handle_add(int fd, const char *qs){
    char title[512]={0}, prbuf[32]={0};
    if(!qget(qs,"title",title,sizeof(title)) || !qget(qs,"priority",prbuf,sizeof(prbuf))){ http_400(fd); return; }
    int pr = atoi(prbuf);
    sqlite3 *db = open_db(); if(!db){ http_500(fd); return; }
    const char *SQL="INSERT INTO todos(title,priority) VALUES(?,?);";
    sqlite3_stmt *st=NULL;
    if(sqlite3_prepare_v2(db,SQL,-1,&st,NULL)!=SQLITE_OK){ sqlite3_close(db); http_500(fd); return; }
    sqlite3_bind_text(st,1,title,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(st,2,pr);
    if(sqlite3_step(st)!=SQLITE_DONE){ sqlite3_finalize(st); sqlite3_close(db); http_500(fd); return; }
    sqlite3_finalize(st);
    long last_id = sqlite3_last_insert_rowid(db);
    sqlite3_close(db);
    char json[128]; snprintf(json,sizeof(json),"{\"status\":\"ok\",\"id\":%ld}", last_id);
    send_json(fd,json);
}

static void handle_done(int fd, const char *qs){
    char idbuf[32]={0}; if(!qget(qs,"id",idbuf,sizeof(idbuf))){ http_400(fd); return; }
    int id=atoi(idbuf);
    sqlite3 *db=open_db(); if(!db){ http_500(fd); return; }
    const char *SQL="UPDATE todos SET done=1 WHERE id=?;";
    sqlite3_stmt *st=NULL;
    if(sqlite3_prepare_v2(db,SQL,-1,&st,NULL)!=SQLITE_OK){ sqlite3_close(db); http_500(fd); return; }
    sqlite3_bind_int(st,1,id);
    if(sqlite3_step(st)!=SQLITE_DONE){ sqlite3_finalize(st); sqlite3_close(db); http_500(fd); return; }
    sqlite3_finalize(st); sqlite3_close(db);
    send_json(fd,"{\"status\":\"ok\"}");
}

static void handle_rm(int fd, const char *qs){
    char idbuf[32]={0}; if(!qget(qs,"id",idbuf,sizeof(idbuf))){ http_400(fd); return; }
    int id=atoi(idbuf);
    sqlite3 *db=open_db(); if(!db){ http_500(fd); return; }
    const char *SQL="DELETE FROM todos WHERE id=?;";
    sqlite3_stmt *st=NULL;
    if(sqlite3_prepare_v2(db,SQL,-1,&st,NULL)!=SQLITE_OK){ sqlite3_close(db); http_500(fd); return; }
    sqlite3_bind_int(st,1,id);
    if(sqlite3_step(st)!=SQLITE_DONE){ sqlite3_finalize(st); sqlite3_close(db); http_500(fd); return; }
    sqlite3_finalize(st); sqlite3_close(db);
    send_json(fd,"{\"status\":\"ok\"}");
}

static void handle_get(int fd, const char *qs){
    char idbuf[32]={0}; if(!qget(qs,"id",idbuf,sizeof(idbuf))){ http_400(fd); return; }
    int id=atoi(idbuf);
    sqlite3 *db=open_db(); if(!db){ http_500(fd); return; }
    const char *SQL="SELECT id,title,priority,done,created_at FROM todos WHERE id=?;";
    sqlite3_stmt *st=NULL;
    if(sqlite3_prepare_v2(db,SQL,-1,&st,NULL)!=SQLITE_OK){ sqlite3_close(db); http_500(fd); return; }
    sqlite3_bind_int(st,1,id);
    int rc = sqlite3_step(st);
    if(rc==SQLITE_ROW){
        char json[1024];
        snprintf(json,sizeof(json),
            "{\"id\":%d,\"title\":\"%s\",\"priority\":%d,\"done\":%d,\"created_at\":\"%s\"}",
            sqlite3_column_int(st,0),
            sqlite3_column_text(st,1),
            sqlite3_column_int(st,2),
            sqlite3_column_int(st,3),
            sqlite3_column_text(st,4));
        send_json(fd,json);
    } else {
        http_404(fd);
    }
    sqlite3_finalize(st); sqlite3_close(db);
}

static void handle_purge(int fd){
    sqlite3 *db=open_db(); if(!db){ http_500(fd); return; }
    char *errmsg=NULL;
    if(sqlite3_exec(db,"DROP TABLE IF EXISTS todos;",NULL,NULL,&errmsg)!=SQLITE_OK){
        if(errmsg){ fprintf(stderr,"sqlite: %s\n",errmsg); sqlite3_free(errmsg); }
        sqlite3_close(db); http_500(fd); return;
    }
    if(sqlite3_exec(db,DDL,NULL,NULL,&errmsg)!=SQLITE_OK){
        if(errmsg){ fprintf(stderr,"sqlite: %s\n",errmsg); sqlite3_free(errmsg); }
        sqlite3_close(db); http_500(fd); return;
    }
    sqlite3_close(db);
    send_json(fd,"{\"status\":\"ok\",\"action\":\"purge\"}");
}

static void serve(int cfd){
    char buf[8192]; ssize_t n = read(cfd, buf, sizeof(buf)-1);
    if(n<=0){ close(cfd); return; } buf[n]='\0';

    char method[8]={0}, path[1024]={0};
    if(sscanf(buf,"%7s %1023s", method, path)!=2){ http_400(cfd); close(cfd); return; }

    char *qs = strchr(path,'?'); if(qs){ *qs++ = '\0'; }

    if(strcmp(method,"GET")!=0 && strcmp(method,"POST")!=0){
        http_400(cfd); close(cfd); return;
    }

    if(strcmp(path,"/list")==0){ handle_list(cfd); }
    else if(strcmp(path,"/add")==0){ handle_add(cfd, qs); }
    else if(strcmp(path,"/done")==0){ handle_done(cfd, qs); }
    else if(strcmp(path,"/rm")==0){ handle_rm(cfd, qs); }
    else if(strcmp(path,"/get")==0){ handle_get(cfd, qs); }
    else if(strcmp(path,"/purge")==0){ handle_purge(cfd); }
    else { http_404(cfd); }

    close(cfd);
}

int main(void){
    int sfd = socket(AF_INET, SOCK_STREAM, 0); if(sfd<0) die("socket");
    int opt=1; setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in addr; memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET; addr.sin_addr.s_addr=htonl(INADDR_ANY); addr.sin_port=htons(8080);
    if(bind(sfd,(struct sockaddr*)&addr,sizeof(addr))<0) die("bind");
    if(listen(sfd,64)<0) die("listen");
    printf("HTTP server listening on http://127.0.0.1:8080\n");
    for(;;){
        int cfd = accept(sfd,NULL,NULL);
        if(cfd<0){ if(errno==EINTR) continue; die("accept"); }
        serve(cfd);
    }
    return 0;
}