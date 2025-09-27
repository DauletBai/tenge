# examples/Makefile
# Build CRUD demo linking libsqlite3 and libcurl.

TEN=../.bin/tenge
RUNTIME=../internal/aotminic/runtime/runtime.c

CFLAGS=-O3 -I../internal/aotminic/runtime
LDFLAGS=-lsqlite3 -lcurl

all: crud

crud: ../.bin/tenge
	$(TEN) -o .bin/crud.c examples/crud_todos/main.tng
	mkdir -p .bin
	cc $(CFLAGS) .bin/crud.c $(RUNTIME) -o .bin/crud $(LDFLAGS)

../.bin/tenge:
	$(MAKE) -C .. build

clean:
	rm -rf .bin