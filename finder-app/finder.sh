#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "Error: filesdir and searchstr must both be specified." >&2
    exit 1
fi

filesdir=$1
searchstr=$2

if [ ! -d "$filesdir" ]; then
    echo "Error: filesdir is not a directory: $filesdir" >&2
    exit 1
fi

file_count=$(find "$filesdir" -type f | wc -l)
matching_line_count=$(grep -r -F -- "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $file_count and the number of matching lines are $matching_line_count"
