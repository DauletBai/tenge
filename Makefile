# Root Makefile for Tenge project

.PHONY: all clean build test bench plot publish help

all: build

clean:
	rm -rf .bin
	rm -rf benchmarks/results
	rm -rf plots
	rm -rf docs/index.html

build:
	@echo "[build] all targets built"
	$(MAKE) -C benchmarks build

test:
	@echo "[test] running tests"
	$(MAKE) -C benchmarks test

bench:
	@echo "[bench] running benchmarks"
	$(MAKE) -C benchmarks bench

plot:
	@echo "[plot] generating plots"
	$(MAKE) -C benchmarks plot

publish:
	@echo "[publish] building index.html for GitHub Pages"
	$(MAKE) -C benchmarks publish

help:
	@echo "Available targets:"
	@echo "  all       - build everything"
	@echo "  clean     - clean all build outputs"
	@echo "  build     - build project and benchmarks"
	@echo "  test      - run tests"
	@echo "  bench     - run benchmarks"
	@echo "  plot      - generate plots"
	@echo "  publish   - publish docs/index.html"
	@echo "  crud-*    - run CRUD demo (C + SQLite)"
	@echo "  crud-http - run CRUD HTTP demo"

# --- CRUD DEMO SHORTCUTS (C + SQLite) ---
.PHONY: crud crud-init crud-add crud-list crud-get crud-done crud-rm crud-purge \
        crud-http crud-http-run

crud:
	@$(MAKE) -C examples/crud_todos build

crud-init:
	@$(MAKE) -C examples/crud_todos init

crud-add:
	@if [ -z "$$title" ] || [ -z "$$priority" ]; then \
	  echo "Usage: make crud-add title='Buy milk' priority=1"; exit 1; \
	fi
	@TITLE="$$title" PRIORITY="$$priority" $(MAKE) -C examples/crud_todos add

crud-list:
	@$(MAKE) -C examples/crud_todos list

crud-get:
	@if [ -z "$$id" ]; then echo "Usage: make crud-get id=1"; exit 1; fi
	@ID="$$id" $(MAKE) -C examples/crud_todos get

crud-done:
	@if [ -z "$$id" ]; then echo "Usage: make crud-done id=1"; exit 1; fi
	@ID="$$id" $(MAKE) -C examples/crud_todos done

crud-rm:
	@if [ -z "$$id" ]; then echo "Usage: make crud-rm id=1"; exit 1; fi
	@ID="$$id" $(MAKE) -C examples/crud_todos rm

crud-purge:
	@$(MAKE) -C examples/crud_todos purge

crud-http:
	@$(MAKE) -C examples/crud_todos http

crud-http-run:
	@$(MAKE) -C examples/crud_todos http-run