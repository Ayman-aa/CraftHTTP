#!/bin/bash

# Initialize counter
count=0

echo "Press any key to test. Press Ctrl+C to stop."

# Trap to handle Ctrl+C and exit gracefully
trap "echo; echo 'Exiting. Total key presses: $count'; exit" SIGINT

# Read keys in a loop
while true; do
    # Use `dd` to capture all key presses, including special ones
    key=$(dd bs=1 count=1 2>/dev/null)
    if [[ -n $key ]]; then
        ((count++))
        echo "Key press detected! Total count: $count"
    fi
done

