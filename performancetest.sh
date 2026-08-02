#!/bin/bash

usage() {
    echo "Usage: $0 -n <n> <program> [args...]"
    echo "  -n <n>       Number of times to run the program"
    echo "  <program>    Path to executable"
    echo "  [args...]    Arguments to pass to the program"
    exit 1
}

# Parse -n flag
if [[ $# -lt 3 ]] || [[ "$1" != "-n" ]]; then
    usage
fi

n=$2
program=$3
shift 3
args=("$@")

# Validate n is a positive integer
if ! [[ "$n" =~ ^[0-9]+$ ]] || [[ "$n" -le 0 ]]; then
    echo "Error: n must be a positive integer"
    exit 1
fi

# Validate program exists
if [[ ! -x "$program" ]]; then
    echo "Error: program '$program' not found or not executable"
    exit 1
fi

# Run program n times and measure total time
start=$(date +%s.%N)
for ((i = 1; i <= n; i++)); do
    "$program" "${args[@]}"
done
end=$(date +%s.%N)

total_time=$(echo "$end - $start" | bc)
avg_time=$(echo "scale=6; $total_time / $n" | bc)

echo "Ran '$program' $n times"
echo "Total execution time: $total_time seconds"
echo "Average execution time: $avg_time seconds"