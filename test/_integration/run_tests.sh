#!/usr/bin/env bash

# To run this file run:
# cd ./test/_integration && chmod +x ./run_tests.sh && ./run_tests.sh

set -euo pipefail

FOLDER="./cases"
COMPILER="./../../cmake-build/app"

for file in "$FOLDER"/*.ocn; do
    echo "Compiling $file..."
    "$COMPILER" "$file" -o "${file%.ocn}.out"

    echo "Running ${file%.ocn}.out with valgrind..."
    
    # Temp file for Valgrind logs
    VALGRIND_LOG=$(mktemp)

    # Run valgrind; do not fail the script on program's exit
    valgrind --leak-check=full --error-exitcode=1 --log-file="$VALGRIND_LOG" "${file%.ocn}.out" &>/dev/null || true

    # Check if valgrind detected errors
    if grep -q "ERROR SUMMARY: [1-9]" "$VALGRIND_LOG"; then
        echo "Valgrind detected memory errors in $file!"
        cat "$VALGRIND_LOG"
        rm -f "$VALGRIND_LOG" "${file%.ocn}.out"
        exit 1
    fi

    # Clean up
    rm -f "$VALGRIND_LOG" "${file%.ocn}.out"
done

echo "All tests passed with no valgrind errors."
