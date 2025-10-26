#!/usr/bin/env python3
import csv
import subprocess
import sys
from pathlib import Path

SRC = Path(__file__).parent / "main.cpp"
BIN = Path(__file__).parent / "nnp"
OUTPUT_CSV = Path(__file__).parent / "results.csv"

DEFAULT_N = 100000  # number of points
DEFAULT_K = 1000  # k nearest
DEFAULT_T = 10000  # number of test query points
RUNS = 5

CONFIGS = [
    {
        "name": "grid_1x1",
        "grid_x": 1.0,
        "grid_y": 1.0,
        "n": DEFAULT_N,
        "k": DEFAULT_K,
        "t": DEFAULT_T,
    },
    {
        "name": "grid_1x10",
        "grid_x": 1.0,
        "grid_y": 10.0,
        "n": DEFAULT_N,
        "k": DEFAULT_K,
        "t": DEFAULT_T,
    },
    {
        "name": "grid_1x100",
        "grid_x": 1.0,
        "grid_y": 100.0,
        "n": DEFAULT_N,
        "k": DEFAULT_K,
        "t": DEFAULT_T,
    },
]


def build_binary():
    print("Building binary...")
    cmd = ["g++", "-std=c++17", "-O2", "-pipe", "-o", str(BIN), str(SRC)]
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        print("Build failed:", e, file=sys.stderr)
        sys.exit(1)


def _parse_output(line: str):
    parts = line.strip().split()
    if len(parts) != 4:
        raise ValueError(f"Unexpected output format: {line}")
    alt_q, long_q, alt_b, long_b = map(float, parts)
    return alt_q, long_q, alt_b, long_b


def run_once(n: int, k: int, t: int, gx: float, gy: float):
    """Run the benchmark binary once and return timing tuple."""
    cmd = [str(BIN), str(n), str(k), str(t), str(gx), str(gy)]
    res = subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    line = res.stdout.strip().splitlines()[-1] if res.stdout else ""
    alt_q, long_q, alt_b, long_b = _parse_output(line)
    return {
        "alt_query_s": alt_q,
        "long_query_s": long_q,
        "alt_build_s": alt_b,
        "long_build_s": long_b,
    }


def main():
    build_binary()

    fieldnames = [
        "id",
        "grid_x",
        "grid_y",
        "n",
        "k",
        "t",
        "alt_build_s",
        "long_build_s",
        "alt_query_s",
        "long_query_s",
    ]

    rows = []
    for cfg in CONFIGS:
        name = cfg["name"]
        gx = cfg["grid_x"]
        gy = cfg["grid_y"]
        n = cfg["n"]
        k = cfg["k"]
        t = cfg["t"]

        print(f"Running {name}")
        for i in range(RUNS):
            r = run_once(n, k, t, gx, gy)
            row = {
                "id": f"{name}_{i}",
                "grid_x": gx,
                "grid_y": gy,
                "n": n,
                "k": k,
                "t": t,
                "alt_build_s": r["alt_build_s"],
                "long_build_s": r["long_build_s"],
                "alt_query_s": r["alt_query_s"],
                "long_query_s": r["long_query_s"],
            }
            rows.append(row)

            print("results:")
            print(f"alt_query_s: {r['alt_query_s']:.6f}s")
            print(f"long_query_s: {r['long_query_s']:.6f}s")
            print(f"alt_build_s: {r['alt_build_s']:.6f}s")
            print(f"long_build_s: {r['long_build_s']:.6f}s")
            print()

    with open(OUTPUT_CSV, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames)
        w.writeheader()
        w.writerows(rows)


if __name__ == "__main__":
    main()
