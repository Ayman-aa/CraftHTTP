#!/bin/bash
if ! command -v xdotool &> /dev/null; then
    echo "Error: xdotool is not installed. Please install it first."
    exit 1
fi
if [ -z "$PASSWORD" ]; then
    echo "Error: Set PASSWORD environment variable first"
    echo "Example: export PASSWORD=your_password"
    exit 1
fi
ft_lock
sleep 1800
xdotool type "$PASSWORD"
xdotool key Return
sleep 1
ft_lock
sleep 1800
xdotool type "$PASSWORD"
xdotool key Return
sleep 1
kill -9 -1   # or
