#!/usr/bin/env python3
"""
Reads multiple CSVs produced by benches, merges them, averages times by (task, lang, variant, N),
and writes:
  - benchmarks/results/results_agg.csv
  - benchmarks/results/RESULTS_AGG.md  (table ordered as Tenge, C, Rust, Go)
The script is lenient to column names: it will look for any of:
  - time columns: TIME_NS, time_ns, avg_ns, TIME_US (will convert to ns)
  - id columns: TASK (or task), LANG (or lang), VARIANT (or variant), N (or size)
Extra columns are ignored.
"""

import sys, os, csv, math
from collections import defaultdict, OrderedDict

PREFERRED_LANG_ORDER = ["tenge", "c", "rust", "go"]

def read_rows(path):
    with open(path, "r", newline="") as f:
        sniffer = csv.Sniffer()
        sample = f.read(4096)
        f.seek(0)
        dialect = sniffer.sniff(sample) if sample else csv.excel
        reader = csv.DictReader(f, dialect=dialect)
        rows = list(reader)
    if not rows:
        return []
    # normalize columns (lowercase keys)
    norm = []
    for r in rows:
        nr = {k.lower().strip(): v for k,v in r.items()}
        norm.append(nr)
    return norm

def parse_time_ns(r):
    # Accept TIME_NS / time_ns / avg_ns / time_us
    for k in ["time_ns", "avg_ns"]:
        if k in r and r[k]:
            try: return float(r[k])
            except: pass
    if "time_us" in r and r["time_us"]:
        try: return float(r["time_us"]) * 1000.0
        except: pass
    # Some runners may store per-repetition columns rep1_ns, rep2_ns...
    reps = [v for k,v in r.items() if k.endswith("_ns") and k.startswith("rep")]
    if reps:
        vals = []
        for v in reps:
            try: vals.append(float(v))
            except: pass
        if vals:
            return sum(vals)/len(vals)
    # As a last resort, 0
    return 0.0

def get(r, keys, default=""):
    for k in keys:
        if k in r and r[k]:
            return r[k]
    return default

def lang_key(lang):
    l = (lang or "").lower()
    try:
        return (0, PREFERRED_LANG_ORDER.index(l))
    except ValueError:
        # non-standard langs go to the end, sorted by name
        return (1, l)

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 benchmarks/aggregate_results.py <csv1> <csv2> <csv3> ...")
        sys.exit(1)

    srcs = sys.argv[1:]
    all_rows = []
    for p in srcs:
        if not os.path.exists(p):
            print(f"[warn] not found: {p}")
            continue
        all_rows.extend(read_rows(p))

    if not all_rows:
        print("[error] no rows read from inputs")
        sys.exit(2)

    # Normalize keys
    agg = defaultdict(list)
    for r in all_rows:
        task    = get(r, ["task"])
        lang    = get(r, ["lang"])
        variant = get(r, ["variant", "var", "algo"])
        size    = get(r, ["n", "size"])

        t_ns = parse_time_ns(r)
        key = (task, lang, variant, size)
        agg[key].append(t_ns)

    # Average
    rows = []
    for (task, lang, variant, size), times in agg.items():
        if not times:
            continue
        mean = sum(times) / len(times)
        # stddev (optional)
        var = sum((x-mean)**2 for x in times) / len(times)
        std = math.sqrt(var)
        rows.append({
            "TASK": task,
            "LANG": lang,
            "VARIANT": variant,
            "N": size,
            "AVG_NS": f"{mean:.0f}",
            "STD_NS": f"{std:.0f}",
            "COUNT": len(times),
        })

    # Sort: (TASK asc), language order: Tenge, C, Rust, Go, then by VARIANT, then by N
    def row_sort_key(r):
        return (r["TASK"], lang_key(r["LANG"] or ""), r["VARIANT"] or "", str(r["N"] or ""))

    rows.sort(key=row_sort_key)

    # Write CSV aggregate
    out_dir = "benchmarks/results"
    os.makedirs(out_dir, exist_ok=True)
    csv_out = os.path.join(out_dir, "results_agg.csv")
    with open(csv_out, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=["TASK","LANG","VARIANT","N","AVG_NS","STD_NS","COUNT"])
        w.writeheader()
        for r in rows:
            w.writerow(r)

    # Build Markdown grouped by TASK with columns in preferred lang order
    md_out = os.path.join(out_dir, "RESULTS_AGG.md")
    # group by TASK -> N -> VARIANT
    grouped = defaultdict(list)
    for r in rows:
        grouped[r["TASK"]].append(r)

    def langs_in_order(items):
        by_lang = defaultdict(list)
        for r in items:
            by_lang[(r["LANG"] or "").lower()].append(r)
        ordered = []
        seen = set()
        for L in PREFERRED_LANG_ORDER:
            ordered.extend(by_lang.get(L, []))
            seen.add(L)
        # dump the rest (if any)
        for k,v in by_lang.items():
            if k not in seen:
                ordered.extend(v)
        return ordered

    with open(md_out, "w") as f:
        f.write("# Aggregated Benchmarks (averaged across provided CSVs)\n\n")
        f.write("_Language columns ordered as: Tenge → C → Rust → Go._\n\n")
        for task, items in grouped.items():
            f.write(f"## {task}\n\n")
            # build a small table: N, VARIANT, then columns per language with AVG_NS (± STD)
            # collect unique (N, VARIANT)
            combos = sorted({ (r["N"], r["VARIANT"]) for r in items },
                            key=lambda x: (str(x[0] or ""), str(x[1] or "")))
            # header
            header = ["N", "Variant"]
            header.extend([L.capitalize() for L in PREFERRED_LANG_ORDER])
            f.write("| " + " | ".join(header) + " |\n")
            f.write("|" + "|".join(["---"]*len(header)) + "|\n")
            for (N, VARIANT) in combos:
                row_cells = [str(N or ""), str(VARIANT or "—")]
                # for each language in order, find matching record
                for L in PREFERRED_LANG_ORDER:
                    matches = [r for r in items if (r["N"]==N and r["VARIANT"]==VARIANT and (r["LANG"] or "").lower()==L)]
                    if matches:
                        r = matches[0]
                        val = f'{int(float(r["AVG_NS"]))} ns'
                        if r["STD_NS"] and r["STD_NS"] != "0":
                            val += f' ± {int(float(r["STD_NS"]))}'
                    else:
                        val = "—"
                    row_cells.append(val)
                f.write("| " + " | ".join(row_cells) + " |\n")
            f.write("\n")

    print(f"[ok] wrote {csv_out}")
    print(f"[ok] wrote {md_out}")
    print("[hint] Commit both to Git: RESULTS_AGG.md is ready for README link.")
    
if __name__ == "__main__":
    main()