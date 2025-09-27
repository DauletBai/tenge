# Build SQLite-based CLI and HTTP server in pure C.

APP      := crud
HTTP_APP := crud_http
DB       := todos.db

# Try pkg-config first, fall back to Homebrew paths on macOS
SQLITE_CFLAGS := $(shell pkg-config --cflags sqlite3 2>/dev/null)
SQLITE_LIBS   := $(shell pkg-config --libs   sqlite3 2>/dev/null)

ifeq ($(strip $(SQLITE_LIBS)),)
SQLITE_CFLAGS := -I/opt/homebrew/include -I/usr/local/include
SQLITE_LIBS   := -L/opt/homebrew/lib -L/usr/local/lib -lsqlite3
endif

CFLAGS := -O3 -Wall -Wextra -Wconversion -std=c11 $(SQLITE_CFLAGS)
LDFLAGS := $(SQLITE_LIBS)

.PHONY: build clean init add list get done rm purge http http-run

build: $(APP)
	@echo "[build] $(APP) ready"

$(APP): crud_host.c schema.sql
	$(CC) $(CFLAGS) crud_host.c -o $(APP) $(LDFLAGS)

init: build
	./$(APP) init

add: build
	@if [ -z "$(TITLE)" ] || [ -z "$(PRIORITY)" ]; then \
		echo "Usage: TITLE='Buy milk' PRIORITY=1 make add"; exit 1; fi
	./$(APP) add "$(TITLE)" $(PRIORITY)

list: build
	./$(APP) list

get: build
	@if [ -z "$(ID)" ]; then echo "Usage: ID=1 make get"; exit 1; fi
	./$(APP) get $(ID)

done: build
	@if [ -z "$(ID)" ]; then echo "Usage: ID=1 make done"; exit 1; fi
	./$(APP) done $(ID)

rm: build
	@if [ -z "$(ID)" ]; then echo "Usage: ID=1 make rm"; exit 1; fi
	./$(APP) rm $(ID)

purge: build
	./$(APP) purge

http: $(HTTP_APP)
	@echo "[build] $(HTTP_APP) ready on :8080"

$(HTTP_APP): http_server.c schema.sql
	$(CC) $(CFLAGS) http_server.c -o $(HTTP_APP) $(LDFLAGS)

http-run: http
	./$(HTTP_APP)

clean:
	rm -f $(APP) $(HTTP_APP) $(DB)