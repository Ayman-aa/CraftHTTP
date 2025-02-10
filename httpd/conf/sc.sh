#!/bin/bash

# Create a temporary file to store count
TEMP_FILE="/tmp/keycount_$$.txt"
echo "0" > "$TEMP_FILE"

# Function to catch keyboard events using xinput
count_keys() {
    xinput test-xi2 --root 3 | \
    while read -r line; do
        if [[ $line == *"EVENT type 2"* ]]; then  # KEY_PRESS events
            count=$(<"$TEMP_FILE")
            echo $((count + 1)) > "$TEMP_FILE"
        fi
    done
}

# Start counting in background
count_keys &
COUNTER_PID=$!

# Function to display current count
show_count() {
    while true; do
        clear
        echo "Keys pressed: $(<"$TEMP_FILE")"
        sleep 1
    done
}

# Start displaying count
show_count &
DISPLAY_PID=$!

# Cleanup function
cleanup() {
    kill $COUNTER_PID 2>/dev/null
    kill $DISPLAY_PID 2>/dev/null
    rm -f "$TEMP_FILE"
    exit 0
}

# Set up cleanup on script exit
trap cleanup INT TERM EXIT

# Keep script running
while true; do
    sleep 1
done
