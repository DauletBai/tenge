# =========================
# Tenge Benchmarks Makefile
# =========================

# -------- Toolchain / flags
CC      := clang
CSTD    := -std=c11
CFLAGS  := -O3 -march=native -ffast-math -fno-math-errno -fno-trapping-math -DNDEBUG -funroll-loops -flto
LDFLAGS := 
GO      := go
CARGO   := cargo

# -------- Paths
BIN_DIR      := .bin
RUNTIME_DIR  := internal/aotminic/runtime
RUNTIME_C    := $(RUNTIME_DIR)/runtime.c
RUNTIME_H    := $(RUNTIME_DIR)/runtime.h
RUNTIME_O    := $(BIN_DIR)/runtime.o

# C sources
C_DIR        := benchmarks/src/c
C_SORT_SRC   := $(C_DIR)/sort.c
C_FIBI_SRC   := $(C_DIR)/fib_iter.c
# C_FIBI_FIXED_SRC := $(C_DIR)/fib_iter_fixed.c  # Removed - file doesn't exist
C_FIBR_SRC   := $(C_DIR)/fib_rec.c
C_VARM_SRC   := $(C_DIR)/var_monte_carlo.c
C_VARA_SRC   := $(C_DIR)/var_mc_acc.c
C_VARA_IMPROVED_SRC := $(C_DIR)/var_mc_acc_improved.c
C_VARA_TENGE_SRC := $(C_DIR)/var_mc_acc_tenge.c
C_SORT_TENGE_SRC := $(C_DIR)/sort_tenge.c
C_NBODY_SRC  := $(C_DIR)/nbody.c
C_NBODYS_SRC := $(C_DIR)/nbody_sym.c
C_YIELD_SRC  := $(C_DIR)/yield_curve.c
C_GARCH_SRC  := $(C_DIR)/garch.c
C_PORTFOLIO_SRC := $(C_DIR)/portfolio_opt.c
C_MATRIX_SRC := $(C_DIR)/matrix_ops.c
C_FFT_SRC    := $(C_DIR)/fft.c

# Go sources (each as a tiny module with main.go)
GO_SORT_DIR   := benchmarks/src/go/sort
GO_FIBI_DIR   := benchmarks/src/go/fib_iter
GO_FIBI_FIXED_DIR := benchmarks/src/go/fib_iter_fixed
GO_FIBR_DIR   := benchmarks/src/go/fib_rec
GO_VARM_DIR   := benchmarks/src/go/var_mc
GO_VARA_DIR   := benchmarks/src/go/var_mc_acc
GO_VARA_IMPROVED_DIR := benchmarks/src/go/var_mc_acc_improved
GO_NBODY_DIR  := benchmarks/src/go/nbody
GO_NBODYS_DIR := benchmarks/src/go/nbody_sym
GO_ENV        := CGO_ENABLED=0

# Rust crates (standalone)
RS_SORT_DIR    := benchmarks/src/rust/sort
RS_FIBI_DIR    := benchmarks/src/rust/fib_iter
RS_FIBI_FIXED_DIR := benchmarks/src/rust/fib_iter_fixed
RS_FIBR_DIR    := benchmarks/src/rust/fib_rec
RS_VARM_DIR    := benchmarks/src/rust/var_mc
RS_VARA_DIR    := benchmarks/src/rust/var_mc_acc
RS_VARA_IMPROVED_DIR := benchmarks/src/rust/var_mc_acc_improved
RS_NBODY_DIR   := benchmarks/src/rust/nbody
RS_NBODYS_DIR  := benchmarks/src/rust/nbody_sym

# Tenge AOT demos (optional — build only if .tng exists)
TNG            := ./.bin/tenge
TNG_SRC_DIR    := benchmarks/src/tenge
TNG_FIBI_TNG   := $(TNG_SRC_DIR)/fib_iter_cli.tng
TNG_FIBR_TNG   := $(TNG_SRC_DIR)/fib_rec_cli.tng
TNG_SORT_QS_TNG:= $(TNG_SRC_DIR)/sort_qsort_cli.tng
TNG_SORT_MS_TNG:= $(TNG_SRC_DIR)/sort_msort_cli.tng
TNG_SORT_PDQ_TNG:=$(TNG_SRC_DIR)/sort_pdq_cli.tng
TNG_SORT_RX_TNG :=$(TNG_SRC_DIR)/sort_radix_cli.tng
TNG_VAR_SORT_TNG:=$(TNG_SRC_DIR)/var_mc_sort_cli.tng
TNG_VAR_ZIG_TNG :=$(TNG_SRC_DIR)/var_mc_zig_cli.tng
TNG_VAR_QS_TNG  :=$(TNG_SRC_DIR)/var_mc_qsel_cli.tng
TNG_VAR_ACC_IMPROVED_TNG :=$(TNG_SRC_DIR)/var_mc_acc_improved_cli.tng
TNG_NBODY_TNG   :=$(TNG_SRC_DIR)/nbody_cli.tng
TNG_NBODYS_TNG  :=$(TNG_SRC_DIR)/nbody_sym_cli.tng

# -------- Binaries that run in run.sh
BIN_TENGE          := $(BIN_DIR)/tenge
BIN_C_SORT         := $(BIN_DIR)/sort_c
BIN_C_FIBI         := $(BIN_DIR)/fib_iter_c
BIN_C_FIBI_FIXED   := $(BIN_DIR)/fib_iter_c_fixed
BIN_C_FIBR         := $(BIN_DIR)/fib_rec_c
BIN_C_VARM         := $(BIN_DIR)/var_mc_c
BIN_C_VARA         := $(BIN_DIR)/var_mc_c_acc
BIN_C_VARA_IMPROVED := $(BIN_DIR)/var_mc_c_acc_improved
BIN_C_VARA_TENGE := $(BIN_DIR)/var_mc_c_acc_tenge
BIN_C_SORT_TENGE := $(BIN_DIR)/sort_tenge
BIN_C_NBODY        := $(BIN_DIR)/nbody_c
BIN_C_NBODYS       := $(BIN_DIR)/nbody_c_sym
BIN_C_YIELD        := $(BIN_DIR)/yield_curve_c
BIN_C_GARCH        := $(BIN_DIR)/garch_c
BIN_C_PORTFOLIO    := $(BIN_DIR)/portfolio_opt_c
BIN_C_MATRIX       := $(BIN_DIR)/matrix_ops_c
BIN_C_FFT          := $(BIN_DIR)/fft_c

BIN_GO_SORT        := $(BIN_DIR)/sort_go
BIN_GO_FIBI        := $(BIN_DIR)/fib_iter_go
BIN_GO_FIBI_FIXED  := $(BIN_DIR)/fib_iter_go_fixed
BIN_GO_FIBR        := $(BIN_DIR)/fib_rec_go
BIN_GO_VARM        := $(BIN_DIR)/var_mc_go
BIN_GO_VARA        := $(BIN_DIR)/var_mc_go_acc
BIN_GO_VARA_IMPROVED := $(BIN_DIR)/var_mc_go_acc_improved
BIN_GO_NBODY       := $(BIN_DIR)/nbody_go
BIN_GO_NBODYS      := $(BIN_DIR)/nbody_go_sym

BIN_RS_SORT        := $(BIN_DIR)/sort_rs
BIN_RS_FIBI        := $(BIN_DIR)/fib_iter_rs
BIN_RS_FIBI_FIXED  := $(BIN_DIR)/fib_iter_rs_fixed
BIN_RS_FIBR        := $(BIN_DIR)/fib_rec_rs
BIN_RS_VARM        := $(BIN_DIR)/var_mc_rs
BIN_RS_VARA        := $(BIN_DIR)/var_mc_rs_acc
BIN_RS_VARA_IMPROVED := $(BIN_DIR)/var_mc_rs_acc_improved
BIN_RS_NBODY       := $(BIN_DIR)/nbody_rs
BIN_RS_NBODYS      := $(BIN_DIR)/nbody_rs_sym

# AOT outputs (C stubs)
BIN_TNG_FIBI_C     := $(BIN_DIR)/fib_cli.c
BIN_TNG_FIBR_C     := $(BIN_DIR)/fib_rec_cli.c
BIN_TNG_SORT_QS_C  := $(BIN_DIR)/sort_cli_qsort.c
BIN_TNG_SORT_MS_C  := $(BIN_DIR)/sort_cli_msort.c
BIN_TNG_SORT_PDQ_C := $(BIN_DIR)/sort_cli_pdq.c
BIN_TNG_SORT_RX_C  := $(BIN_DIR)/sort_cli_radix.c
BIN_TNG_VARM_SORT_C:= $(BIN_DIR)/var_mc_tng_sort.c
BIN_TNG_VARM_ZIG_C := $(BIN_DIR)/var_mc_tng_zig.c
BIN_TNG_VARM_QS_C  := $(BIN_DIR)/var_mc_tng_qsel.c
BIN_TNG_VARM_ACC_IMPROVED_C := $(BIN_DIR)/var_mc_tng_acc_improved.c
BIN_TNG_NBODY_C    := $(BIN_DIR)/nbody_tng.c
BIN_TNG_NBODYS_C   := $(BIN_DIR)/nbody_tng_sym.c

# Tenge binary executables
BIN_TNG_FIBI       := $(BIN_DIR)/fib_cli
BIN_TNG_FIBR       := $(BIN_DIR)/fib_rec_cli
BIN_TNG_SORT_QS    := $(BIN_DIR)/sort_cli_qsort
BIN_TNG_SORT_MS    := $(BIN_DIR)/sort_cli_msort
BIN_TNG_SORT_PDQ   := $(BIN_DIR)/sort_cli_pdq
BIN_TNG_SORT_RX    := $(BIN_DIR)/sort_cli_radix
BIN_TNG_VARM_SORT  := $(BIN_DIR)/var_mc_tng_sort
BIN_TNG_VARM_ZIG   := $(BIN_DIR)/var_mc_tng_zig
BIN_TNG_VARM_QS    := $(BIN_DIR)/var_mc_tng_qsel
BIN_TNG_VARM_ACC_IMPROVED := $(BIN_DIR)/var_mc_tng_acc_improved
BIN_TNG_NBODY      := $(BIN_DIR)/nbody_tng
BIN_TNG_NBODYS     := $(BIN_DIR)/nbody_tng_sym

# =========================
# Top-level phony targets
# =========================
.PHONY: all clean build cbenches gobenches rustbenches aot tengebenches

all: build
build: $(BIN_TENGE) cbenches gobenches rustbenches aot tengebenches
	@echo "[build] All targets built."

clean:
	@rm -rf $(BIN_DIR)
	@mkdir -p $(BIN_DIR)
	@echo "[clean] $(BIN_DIR) removed."

# =========================
# Compiler itself (Go)
# =========================
$(BIN_TENGE):
	@echo "[build] compiler -> $(BIN_TENGE)"
	@$(GO) build -o $(BIN_TENGE) ./cmd/tenge

# =========================
# Common runtime (C)
# =========================
$(RUNTIME_O): $(RUNTIME_C) $(RUNTIME_H) | $(BIN_DIR)
	@echo "[build] runtime.o built"
	@$(CC) $(CSTD) $(CFLAGS) -c $(RUNTIME_C) -o $(RUNTIME_O)

# =========================
# C benchmarks
# =========================
cbenches: $(BIN_C_SORT) $(BIN_C_SORT_TENGE) $(BIN_C_FIBI) $(BIN_C_FIBR) $(BIN_C_VARM) $(BIN_C_VARA) $(BIN_C_VARA_IMPROVED) $(BIN_C_VARA_TENGE) $(BIN_C_NBODY) $(BIN_C_NBODYS) $(BIN_C_YIELD) $(BIN_C_GARCH) $(BIN_C_PORTFOLIO) $(BIN_C_MATRIX) $(BIN_C_FFT)
	@echo "[build] C benches built"

$(BIN_C_SORT): $(C_SORT_SRC) $(RUNTIME_O)
	@echo "[build] c_sort -> $(BIN_C_SORT)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_SORT_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_SORT) $(LDFLAGS)

$(BIN_C_SORT_TENGE): $(C_SORT_TENGE_SRC) $(RUNTIME_O)
	@echo "[build] c_sort_tenge -> $(BIN_C_SORT_TENGE)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_SORT_TENGE_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_SORT_TENGE) $(LDFLAGS)

$(BIN_C_YIELD): $(C_YIELD_SRC) $(RUNTIME_O)
	@echo "[build] c_yield_curve -> $(BIN_C_YIELD)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_YIELD_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_YIELD) $(LDFLAGS) -lm

$(BIN_C_GARCH): $(C_GARCH_SRC) $(RUNTIME_O)
	@echo "[build] c_garch -> $(BIN_C_GARCH)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_GARCH_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_GARCH) $(LDFLAGS) -lm

$(BIN_C_PORTFOLIO): $(C_PORTFOLIO_SRC) $(RUNTIME_O)
	@echo "[build] c_portfolio_opt -> $(BIN_C_PORTFOLIO)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_PORTFOLIO_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_PORTFOLIO) $(LDFLAGS) -lm

$(BIN_C_MATRIX): $(C_MATRIX_SRC) $(RUNTIME_O)
	@echo "[build] c_matrix_ops -> $(BIN_C_MATRIX)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_MATRIX_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_MATRIX) $(LDFLAGS) -lm

$(BIN_C_FFT): $(C_FFT_SRC) $(RUNTIME_O)
	@echo "[build] c_fft -> $(BIN_C_FFT)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_FFT_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_FFT) $(LDFLAGS) -lm

$(BIN_C_FIBI): $(C_FIBI_SRC) $(RUNTIME_O)
	@echo "[build] c_fib_iter -> $(BIN_C_FIBI)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_FIBI_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_FIBI) $(LDFLAGS)

$(BIN_C_FIBI_FIXED): $(C_FIBI_FIXED_SRC) $(RUNTIME_O)
	@echo "[build] c_fib_iter_fixed -> $(BIN_C_FIBI_FIXED)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_FIBI_FIXED_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_FIBI_FIXED) $(LDFLAGS)

$(BIN_C_FIBR): $(C_FIBR_SRC) $(RUNTIME_O)
	@echo "[build] c_fib_rec -> $(BIN_C_FIBR)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_FIBR_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_FIBR) $(LDFLAGS)

$(BIN_C_VARM): $(C_VARM_SRC) $(RUNTIME_O)
	@echo "[build] c_var_mc -> $(BIN_C_VARM)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_VARM_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_VARM) $(LDFLAGS)

$(BIN_C_VARA): $(C_VARA_SRC) $(RUNTIME_O)
	@echo "[build] c_var_mc_acc -> $(BIN_C_VARA)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_VARA_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_VARA) $(LDFLAGS)

$(BIN_C_VARA_IMPROVED): $(C_VARA_IMPROVED_SRC) $(RUNTIME_O)
	@echo "[build] c_var_mc_acc_improved -> $(BIN_C_VARA_IMPROVED)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_VARA_IMPROVED_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_VARA_IMPROVED) $(LDFLAGS)

$(BIN_C_VARA_TENGE): $(C_VARA_TENGE_SRC) $(RUNTIME_O)
	@echo "[build] c_var_mc_acc_tenge -> $(BIN_C_VARA_TENGE)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_VARA_TENGE_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_VARA_TENGE) $(LDFLAGS)

$(BIN_C_NBODY): $(C_NBODY_SRC) $(RUNTIME_O)
	@echo "[build] c_nbody -> $(BIN_C_NBODY)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_NBODY_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_NBODY) $(LDFLAGS)

$(BIN_C_NBODYS): $(C_NBODYS_SRC) $(RUNTIME_O)
	@echo "[build] c_nbody_sym -> $(BIN_C_NBODYS)"
	@$(CC) $(CSTD) $(CFLAGS) $(C_NBODYS_SRC) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_C_NBODYS) $(LDFLAGS)

# =========================
# Go benchmarks (build only if folders exist)
# =========================
gobenches: $(BIN_GO_SORT) $(BIN_GO_FIBI) $(BIN_GO_FIBI_FIXED) $(BIN_GO_FIBR) $(BIN_GO_VARM) $(BIN_GO_VARA) $(BIN_GO_VARA_IMPROVED) $(BIN_GO_NBODY) $(BIN_GO_NBODYS)

$(BIN_GO_SORT):
	@if [ -d "$(GO_SORT_DIR)" ]; then \
		echo "[build] go_sort -> $(BIN_GO_SORT)"; \
		cd $(GO_SORT_DIR) && $(GO) build -o ../../../..//$(BIN_GO_SORT) . ; \
	else \
		echo "  [skip] $(GO_SORT_DIR) not found"; \
	fi

$(BIN_GO_NBODY):
	@if [ -d "$(GO_NBODY_DIR)" ]; then \
		echo "[build] go_nbody -> $(BIN_GO_NBODY)"; \
		cd $(GO_NBODY_DIR) && $(GO) build -o ../../../..//$(BIN_GO_NBODY) . ; \
	else \
		echo "  [skip] $(GO_NBODY_DIR) not found"; \
	fi

$(BIN_GO_NBODYS):
	@if [ -d "$(GO_NBODYS_DIR)" ]; then \
		echo "[build] go_nbody_sym -> $(BIN_GO_NBODYS)"; \
		cd $(GO_NBODYS_DIR) && $(GO) build -o ../../../..//$(BIN_GO_NBODYS) . ; \
	else \
		echo "  [skip] $(GO_NBODYS_DIR) not found"; \
	fi

$(BIN_GO_FIBI):
	@if [ -d "$(GO_FIBI_DIR)" ]; then \
		echo "[build] go_fib_iter -> $(BIN_GO_FIBI)"; \
		cd $(GO_FIBI_DIR) && $(GO) build -o ../../../..//$(BIN_GO_FIBI) . ; \
	else \
		echo "  [skip] $(GO_FIBI_DIR) not found"; \
	fi

$(BIN_GO_FIBI_FIXED):
	@if [ -d "$(GO_FIBI_FIXED_DIR)" ]; then \
		echo "[build] go_fib_iter_fixed -> $(BIN_GO_FIBI_FIXED)"; \
		cd $(GO_FIBI_FIXED_DIR) && $(GO) build -o ../../../..//$(BIN_GO_FIBI_FIXED) . ; \
	else \
		echo "  [skip] $(GO_FIBI_FIXED_DIR) not found"; \
	fi

$(BIN_GO_FIBR):
	@if [ -d "$(GO_FIBR_DIR)" ]; then \
		echo "[build] go_fib_rec -> $(BIN_GO_FIBR)"; \
		cd $(GO_FIBR_DIR) && $(GO) build -o ../../../..//$(BIN_GO_FIBR) . ; \
	else \
		echo "  [skip] $(GO_FIBR_DIR) not found"; \
	fi

$(BIN_GO_VARM):
	@if [ -d "$(GO_VARM_DIR)" ]; then \
		echo "[build] go_var_mc -> $(BIN_GO_VARM)"; \
		cd $(GO_VARM_DIR) && $(GO) build -o ../../../..//$(BIN_GO_VARM) . ; \
	else \
		echo "  [skip] $(GO_VARM_DIR) not found"; \
	fi

$(BIN_GO_VARA):
	@if [ -d "$(GO_VARA_DIR)" ]; then \
		echo "[build] go_var_mc_acc -> $(BIN_GO_VARA)"; \
		cd $(GO_VARA_DIR) && $(GO) build -o ../../../..//$(BIN_GO_VARA) . ; \
	else \
		echo "  [skip] $(GO_VARA_DIR) not found"; \
	fi

$(BIN_GO_VARA_IMPROVED):
	@if [ -d "$(GO_VARA_IMPROVED_DIR)" ]; then \
		echo "[build] go_var_mc_acc_improved -> $(BIN_GO_VARA_IMPROVED)"; \
		cd $(GO_VARA_IMPROVED_DIR) && $(GO) build -o ../../../..//$(BIN_GO_VARA_IMPROVED) . ; \
	else \
		echo "  [skip] $(GO_VARA_IMPROVED_DIR) not found"; \
	fi

# =========================
# Rust benchmarks (per crate, only if Cargo.toml exists)
# =========================
rustbenches: $(BIN_RS_SORT) $(BIN_RS_FIBI) $(BIN_RS_FIBI_FIXED) $(BIN_RS_FIBR) $(BIN_RS_VARM) $(BIN_RS_VARA) $(BIN_RS_VARA_IMPROVED) $(BIN_RS_NBODY) $(BIN_RS_NBODYS)

define RS_BUILD_RULE
$(1):
	@if [ -f "$(2)/Cargo.toml" ]; then \
		echo "[build] rust $(3) (per-crate) -> $@" ; \
		cd $(2) && $(CARGO) build --release >/dev/null ; \
		if [ -f "target/release/$(3)" ]; then \
			cp "target/release/$(3)" ../../../../$(1) ; \
		else \
			echo "  [skip] $(2) built, but binary '$(3)' not found"; \
		fi ; \
	else \
		echo "  [skip] $(2) not found"; \
	fi
endef

$(eval $(call RS_BUILD_RULE,$(BIN_RS_SORT),$(RS_SORT_DIR),sort_rs))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_FIBI),$(RS_FIBI_DIR),fib_iter))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_FIBI_FIXED),$(RS_FIBI_FIXED_DIR),fib_iter_fixed))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_FIBR),$(RS_FIBR_DIR),fib_rec))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_VARM),$(RS_VARM_DIR),var_mc))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_VARA),$(RS_VARA_DIR),var_mc_acc))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_VARA_IMPROVED),$(RS_VARA_IMPROVED_DIR),var_mc_acc_improved))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_NBODY),$(RS_NBODY_DIR),nbody))
$(eval $(call RS_BUILD_RULE,$(BIN_RS_NBODYS),$(RS_NBODYS_DIR),nbody_sym))

# =========================
# Tenge AOT (generate C stubs only if .tng exists)
# =========================
aot: $(BIN_TENGE) aot_fibi aot_var aot_nbody

aot_fibi:
	@if [ -f "$(TNG_FIBI_TNG)" ]; then echo "[aot] fib_iter -> $(BIN_TNG_FIBI_C)"; $(BIN_TENGE) -o $(BIN_TNG_FIBI_C) $(TNG_FIBI_TNG); else echo "[aot] skip (not found): $(TNG_FIBI_TNG)"; fi
	@if [ -f "$(TNG_FIBR_TNG)" ]; then echo "[aot] fib_rec -> $(BIN_TNG_FIBR_C)"; $(BIN_TENGE) -o $(BIN_TNG_FIBR_C) $(TNG_FIBR_TNG); else echo "[aot] skip (not found): $(TNG_FIBR_TNG)"; fi

aot_sort:
	@if [ -f "$(TNG_SORT_QS_TNG)" ]; then echo "[aot] sort_qsort -> $(BIN_TNG_SORT_QS_C)"; $(BIN_TENGE) -o $(BIN_TNG_SORT_QS_C) $(TNG_SORT_QS_TNG); else echo "[aot] skip (not found): $(TNG_SORT_QS_TNG)"; fi
	@if [ -f "$(TNG_SORT_MS_TNG)" ]; then echo "[aot] sort_msort -> $(BIN_TNG_SORT_MS_C)"; $(BIN_TENGE) -o $(BIN_TNG_SORT_MS_C) $(TNG_SORT_MS_TNG); else echo "[aot] skip (not found): $(TNG_SORT_MS_TNG)"; fi
	@if [ -f "$(TNG_SORT_PDQ_TNG)" ]; then echo "[aot] sort_pdq -> $(BIN_TNG_SORT_PDQ_C)"; $(BIN_TENGE) -o $(BIN_TNG_SORT_PDQ_C) $(TNG_SORT_PDQ_TNG); else echo "[aot] skip (not found): $(TNG_SORT_PDQ_TNG)"; fi
	@if [ -f "$(TNG_SORT_RX_TNG)" ]; then echo "[aot] sort_radix -> $(BIN_TNG_SORT_RX_C)"; $(BIN_TENGE) -o $(BIN_TNG_SORT_RX_C) $(TNG_SORT_RX_TNG); else echo "[aot] skip (not found): $(TNG_SORT_RX_TNG)"; fi

aot_var:
	@if [ -f "$(TNG_VAR_SORT_TNG)" ]; then echo "[aot] var_mc_sort -> $(BIN_TNG_VARM_SORT_C)"; $(BIN_TENGE) -o $(BIN_TNG_VARM_SORT_C) $(TNG_VAR_SORT_TNG); else echo "[aot] skip (not found): $(TNG_VAR_SORT_TNG)"; fi
	@if [ -f "$(TNG_VAR_ZIG_TNG)" ]; then echo "[aot] var_mc_zig -> $(BIN_TNG_VARM_ZIG_C)"; $(BIN_TENGE) -o $(BIN_TNG_VARM_ZIG_C) $(TNG_VAR_ZIG_TNG); else echo "[aot] skip (not found): $(TNG_VAR_ZIG_TNG)"; fi
	@if [ -f "$(TNG_VAR_QS_TNG)" ]; then echo "[aot] var_mc_qsel -> $(BIN_TNG_VARM_QS_C)"; $(BIN_TENGE) -o $(BIN_TNG_VARM_QS_C) $(TNG_VAR_QS_TNG); else echo "[aot] skip (not found): $(TNG_VAR_QS_TNG)"; fi

aot_nbody:
	@if [ -f "$(TNG_NBODY_TNG)" ]; then echo "[aot] nbody -> $(BIN_TNG_NBODY_C)"; $(BIN_TENGE) -o $(BIN_TNG_NBODY_C) $(TNG_NBODY_TNG); else echo "[aot] skip (not found): $(TNG_NBODY_TNG)"; fi
	@if [ -f "$(TNG_NBODYS_TNG)" ]; then echo "[aot] nbody_sym -> $(BIN_TNG_NBODYS_C)"; $(BIN_TENGE) -o $(BIN_TNG_NBODYS_C) $(TNG_NBODYS_TNG); else echo "[aot] skip (not found): $(TNG_NBODYS_TNG)"; fi

# =========================
# Tenge benchmarks (compile C stubs to binaries)
# =========================
tengebenches: $(BIN_TNG_FIBI) $(BIN_TNG_FIBR) $(BIN_TNG_VARM_SORT) $(BIN_TNG_VARM_ZIG) $(BIN_TNG_VARM_QS) $(BIN_TNG_NBODY) $(BIN_TNG_NBODYS)

$(BIN_TNG_FIBI): $(BIN_TNG_FIBI_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_FIBI_C)" ]; then \
		echo "[build] tenge_fib_iter -> $(BIN_TNG_FIBI)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_FIBI_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_FIBI) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_FIBI_C) not found"; \
	fi

$(BIN_TNG_FIBR): $(BIN_TNG_FIBR_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_FIBR_C)" ]; then \
		echo "[build] tenge_fib_rec -> $(BIN_TNG_FIBR)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_FIBR_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_FIBR) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_FIBR_C) not found"; \
	fi

$(BIN_TNG_SORT_QS): $(BIN_TNG_SORT_QS_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_SORT_QS_C)" ]; then \
		echo "[build] tenge_sort_qsort -> $(BIN_TNG_SORT_QS)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_SORT_QS_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_SORT_QS) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_SORT_QS_C) not found"; \
	fi

$(BIN_TNG_SORT_MS): $(BIN_TNG_SORT_MS_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_SORT_MS_C)" ]; then \
		echo "[build] tenge_sort_msort -> $(BIN_TNG_SORT_MS)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_SORT_MS_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_SORT_MS) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_SORT_MS_C) not found"; \
	fi

$(BIN_TNG_SORT_PDQ): $(BIN_TNG_SORT_PDQ_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_SORT_PDQ_C)" ]; then \
		echo "[build] tenge_sort_pdq -> $(BIN_TNG_SORT_PDQ)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_SORT_PDQ_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_SORT_PDQ) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_SORT_PDQ_C) not found"; \
	fi

$(BIN_TNG_SORT_RX): $(BIN_TNG_SORT_RX_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_SORT_RX_C)" ]; then \
		echo "[build] tenge_sort_radix -> $(BIN_TNG_SORT_RX)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_SORT_RX_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_SORT_RX) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_SORT_RX_C) not found"; \
	fi

$(BIN_TNG_VARM_SORT): $(BIN_TNG_VARM_SORT_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_VARM_SORT_C)" ]; then \
		echo "[build] tenge_var_mc_sort -> $(BIN_TNG_VARM_SORT)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_VARM_SORT_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_VARM_SORT) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_VARM_SORT_C) not found"; \
	fi

$(BIN_TNG_VARM_ZIG): $(BIN_TNG_VARM_ZIG_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_VARM_ZIG_C)" ]; then \
		echo "[build] tenge_var_mc_zig -> $(BIN_TNG_VARM_ZIG)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_VARM_ZIG_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_VARM_ZIG) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_VARM_ZIG_C) not found"; \
	fi

$(BIN_TNG_VARM_QS): $(BIN_TNG_VARM_QS_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_VARM_QS_C)" ]; then \
		echo "[build] tenge_var_mc_qsel -> $(BIN_TNG_VARM_QS)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_VARM_QS_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_VARM_QS) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_VARM_QS_C) not found"; \
	fi

$(BIN_TNG_VARM_ACC_IMPROVED): $(BIN_TNG_VARM_ACC_IMPROVED_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_VARM_ACC_IMPROVED_C)" ]; then \
		echo "[build] tenge_var_mc_acc_improved -> $(BIN_TNG_VARM_ACC_IMPROVED)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_VARM_ACC_IMPROVED_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_VARM_ACC_IMPROVED) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_VARM_ACC_IMPROVED_C) not found"; \
	fi

$(BIN_TNG_NBODY): $(BIN_TNG_NBODY_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_NBODY_C)" ]; then \
		echo "[build] tenge_nbody -> $(BIN_TNG_NBODY)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_NBODY_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_NBODY) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_NBODY_C) not found"; \
	fi

$(BIN_TNG_NBODYS): $(BIN_TNG_NBODYS_C) $(RUNTIME_O)
	@if [ -f "$(BIN_TNG_NBODYS_C)" ]; then \
		echo "[build] tenge_nbody_sym -> $(BIN_TNG_NBODYS)"; \
		$(CC) $(CSTD) $(CFLAGS) $(BIN_TNG_NBODYS_C) $(RUNTIME_O) -I$(RUNTIME_DIR) -o $(BIN_TNG_NBODYS) $(LDFLAGS); \
	else \
		echo "  [skip] $(BIN_TNG_NBODYS_C) not found"; \
	fi