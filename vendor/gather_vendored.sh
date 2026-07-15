#!/usr/bin/env bash
set -euo pipefail

VENDOR_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMBINED="$VENDOR_ROOT/vendor.gvf"

: > "$COMBINED"

for module_script in "$VENDOR_ROOT"/*/vendor.sh; do
    module_dir="$(dirname "$module_script")"
    module_name="$(basename "$module_dir")"

    echo "=== [$module_name] ==="
    "$module_script" "$@"

    shopt -s nullglob
    flags_files=("$module_dir"/*.gvf)
    shopt -u nullglob

    if [ "${#flags_files[@]}" -eq 0 ]; then
        echo "[vendor] WARNING: $module_name produced no .glint_vendor_flags file" >&2
        continue
    fi

    for f in "${flags_files[@]}"; do
        cat "$f" >> "$COMBINED"
    done
done

echo "[vendor] wrote $COMBINED"
