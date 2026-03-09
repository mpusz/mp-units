#!/usr/bin/env python3
"""Generate GitHub Actions step summary for the CI build matrix.

Reads the matrix JSON from the MATRIX_JSON environment variable (or stdin as
fallback) and the random seed from MATRIX_SEED, then writes a Markdown summary
suitable for appending to $GITHUB_STEP_SUMMARY.
"""

import json
import os
import sys


def yn(val: str) -> str:
    return "✅" if val == "True" else "—"


def generate_matrix_summary(matrix: list[dict], seed: int | str | None = None) -> str:
    lines = []

    lines.append("## 🎯 Build Matrix")
    lines.append("")

    meta = []
    if seed is not None:
        meta.append(f"**Random seed:** `{seed}`")
    meta.append(f"**Total jobs:** {len(matrix)}")
    lines.extend(meta)
    lines.append("")

    # Main combinations table
    lines.append(
        "| Toolchain | C++ | Formatting | Contracts | Build"
        " | Modules | import_std | no_crtp | freestanding |"
    )
    lines.append(
        "|-----------|-----|------------|-----------|-------|"
        ":-------:|:----------:|:-------:|:------------:|"
    )
    for entry in sorted(matrix, key=lambda e: e["config-summary-str"]):
        t = entry["toolchain"]
        lines.append(
            f"| {t['name']}"
            f" | {entry['std']}"
            f" | {entry['formatting']}"
            f" | {entry['contracts']}"
            f" | {entry['build_type']}"
            f" | {yn(entry['cxx_modules'])}"
            f" | {yn(entry['import_std'])}"
            f" | {yn(entry['no_crtp'])}"
            f" | {yn(entry['freestanding'])} |"
        )

    lines.append("")

    # Per-value coverage statistics in a collapsible block
    lines.append("<details><summary>📊 Coverage statistics</summary>")
    lines.append("")
    lines.append("```")

    field_getters: list[tuple[str, object]] = [
        ("toolchain", lambda e: e["toolchain"]["name"]),
        ("std", lambda e: str(e["std"])),
        ("build_type", lambda e: e["build_type"]),
        ("contracts", lambda e: e["contracts"]),
        ("cxx_modules", lambda e: e["cxx_modules"]),
        ("std_format", lambda e: e["std_format"]),
        ("import_std", lambda e: e["import_std"]),
        ("no_crtp", lambda e: e["no_crtp"]),
        ("freestanding", lambda e: e["freestanding"]),
    ]
    for key, getter in field_getters:
        counts: dict[str, int] = {}
        for entry in matrix:
            v = getter(entry)
            counts[v] = counts.get(v, 0) + 1
        for v, n in sorted(counts.items()):
            lines.append(f"  {key}={v}: {n}")

    lines.append("```")
    lines.append("</details>")

    return "\n".join(lines)


def main():
    matrix_json = os.environ.get("MATRIX_JSON") or sys.stdin.read().strip()
    seed = os.environ.get("MATRIX_SEED")

    if not matrix_json:
        print(
            "Error: No matrix JSON provided (set MATRIX_JSON env var or pipe to stdin)",
            file=sys.stderr,
        )
        sys.exit(1)

    try:
        matrix = json.loads(matrix_json)
        print(generate_matrix_summary(matrix, seed=seed))
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
