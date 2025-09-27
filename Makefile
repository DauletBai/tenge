SHELL := /bin/bash
.PHONY: all clean build c_benches go_benches rust_benches tenge_cli benches

BIN := .bin
$(shell mkdir -p $(BIN))

# Profiles
PROFILE ?= strict
CFLAGS_STRICT := -O3
CFLAGS_FAST   := -O3 -ffast-math -funroll-loops
CFLAGS := $(if $(filter $(PROFILE),fast),$(CFLAGS_FAST),$(CFLAGS_STRICT))

RUSTFLAGS_STRICT :=
RUSTFLAGS_FAST   := -C opt-level=3
export RUSTFLAGS := $(if $(filter $(PROFILE),fast),$(RUSTFLAGS_FAST),$(RUSTFLAGS_STRICT))

GO := go

all: build

clean:
	rm -rf $(BIN)
	mkdir -p $(BIN)
	@echo "[clean] .bin removed and recreated."

build: $(BIN)/tenge runtime.o c_benches go_benches rust_benches tenge_cli
	@echo "[build] All targets built."

$(BIN)/tenge:
	$(GO) build -o $(BIN)/tenge ./cmd/tenge
	@echo "[build] compiler -> $(BIN)/tenge"

runtime.o:
	cc -O3 -c internal/aotminic/runtime/runtime.c -o $(BIN)/runtime.o
	@echo "[build] runtime.o built"

################# C benches ##################
define CC_ONE
cc $(CFLAGS) -Iinternal/aotminic/runtime benchmarks/src/c/$(1).c $(BIN)/runtime.o -o $(BIN)/$(2)
	@echo "[build] $(BIN)/$(2)"
endef

c_benches:
	$(call CC_ONE,fft,fft_c)
	$(call CC_ONE,fib_iter,fib_iter_c)
	$(call CC_ONE,fib_rec,fib_rec_c)
	$(call CC_ONE,garch,garch_c)
	$(call CC_ONE,matrix_ops,matrix_ops_c)
	$(call CC_ONE,nbody_sym_crossplatform,nbody_sym_crossplatform_c)
	$(call CC_ONE,nbody_sym,nbody_sym_c)
	$(call CC_ONE,nbody,nbody_c)
	$(call CC_ONE,portfolio_opt_agglutinative,portfolio_opt_agglutinative_c)
	$(call CC_ONE,portfolio_opt,portfolio_opt_c)
	$(call CC_ONE,sort_tenge,sort_tenge_c)
	$(call CC_ONE,sort,sort_c)
	$(call CC_ONE,var_mc_acc_improved,var_mc_acc_improved_c)
	$(call CC_ONE,var_mc_acc_tenge,var_mc_acc_tenge_c)
	$(call CC_ONE,var_mc_acc,var_mc_acc_c)
	$(call CC_ONE,var_monte_carlo,var_monte_carlo_c)
	$(call CC_ONE,yield_curve,yield_curve_c)
	@echo "[build] C benches built"

################# Go benches #################
define GO_ONE
	$(GO) build -o $(BIN)/$(2) ./benchmarks/src/go/$(1)
	@echo "[build] $(BIN)/$(2)"
endef

go_benches:
	@echo "[build] Go benches…"
	$(call GO_ONE,fft,fft)
	$(call GO_ONE,fib_iter,fib_iter)
	$(call GO_ONE,fib_rec,fib_rec)
	$(call GO_ONE,garch,garch)
	$(call GO_ONE,matrix_ops,matrix_ops)
	$(call GO_ONE,nbody,nbody)
	$(call GO_ONE,nbody_sym,nbody_sym)
	$(call GO_ONE,portfolio_opt,portfolio_opt)
	$(call GO_ONE,sort,sort)
	$(call GO_ONE,var_mc,var_mc)
	$(call GO_ONE,var_mc_acc,var_mc_acc)
	$(call GO_ONE,var_mc_acc_improved,var_mc_acc_improved)
	$(call GO_ONE,yield_curve,yield_curve)
	@echo "[build] Go benches done (13 built)"

############### Rust benches #################
define RUST_ONE
	cd benchmarks/src/rust/$(1) && cargo build --release
	cp benchmarks/src/rust/$(1)/target/release/$(2) $(BIN)/$(3)
	@echo "[build] $(BIN)/$(3)"
endef

rust_benches:
	@echo "[build] Rust benches…"
	$(call RUST_ONE,sort,sort,sort_rs)
	$(call RUST_ONE,fib_iter,fib_iter,fib_iter_rs)
	$(call RUST_ONE,fib_rec,fib_rec,fib_rec_rs)
	$(call RUST_ONE,var_mc,var_mc,var_mc_rs)
	$(call RUST_ONE,nbody,nbody,nbody_rs)
	$(call RUST_ONE,nbody_sym,nbody_sym,nbody_rs_sym)
	@echo "[build] Rust benches done"

############### Tenge AOT CLIs ################
define TENGE_ONE
	$(BIN)/tenge -o $(BIN)/$(1).c benchmarks/src/tenge/$(2).tng
	cc $(CFLAGS) -Iinternal/aotminic/runtime $(BIN)/$(1).c internal/aotminic/runtime/runtime.c -o $(BIN)/$(1)
	@echo "[build] $(BIN)/$(1)"
endef

tenge_cli:
	$(call TENGE_ONE,sort_cli_qsort,sort_qsort_cli)
	$(call TENGE_ONE,sort_cli_msort,sort_msort_cli)
	$(call TENGE_ONE,sort_cli_pdq,sort_pdq_cli)
	$(call TENGE_ONE,sort_cli_radix,sort_radix_cli)
	$(call TENGE_ONE,var_mc_tng_sort,var_mc_sort_cli)
	$(call TENGE_ONE,var_mc_tng_zig,var_mc_zig_cli)
	$(call TENGE_ONE,var_mc_tng_qsel,var_mc_qsel_cli)
	$(call TENGE_ONE,fib_cli,fib_iter_cli)
	$(call TENGE_ONE,fib_rec_cli,fib_rec_cli)
	$(call TENGE_ONE,nbody_tng,nbody_cli)
	$(call TENGE_ONE,nbody_tng_sym,nbody_sym_cli)

benches: build
	./benchmarks/run.sh