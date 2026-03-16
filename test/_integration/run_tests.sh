#!/usr/bin/env bash

set -euo pipefail

FOLDER="./cases"
COMPILER="./../../cmake-build/app"
LOGFILE="./valgrind.log"

# Clear logfile at start
> "$LOGFILE"

echo "Starting integration tests..."
echo "Logfile: $LOGFILE"

for file in "$FOLDER"/*.ocn; do
    echo "----------------------------------------"
    echo "Compiling $file..."

    OUTFILE="${file%.ocn}.out"
    "$COMPILER" "$file" -o "$OUTFILE"

    echo "Running with valgrind..."

    {
        echo ""
        echo "========================================"
        echo "Test: $file"
        echo "Time: $(date)"
        echo "========================================"

        valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            "$OUTFILE"
    } >> "$LOGFILE" 2>&1 || true

    if grep -q "ERROR SUMMARY: [1-9]" "$LOGFILE"; then
        echo "Valgrind detected memory errors in $file"
        echo "Check logfile: $LOGFILE"
        rm -f "$OUTFILE"
        exit 1
    fi

    echo "$file passed"
    rm -f "$OUTFILE"
done

echo "----------------------------------------"
echo "All tests passed."
echo "Full log: $LOGFILE"