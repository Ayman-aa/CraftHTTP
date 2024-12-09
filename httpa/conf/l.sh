#!/bin/bash

# Ensure xdotool is available
if ! command -v xdotool &> /dev/null; then
    echo "Error: xdotool is not installed. Please install it first."
    exit 1
fi

# Check if PASSWORD is set
if [ -z "$PASSWORD" ]; then
    echo "Error: Set PASSWORD environment variable first"
    echo "Example: export PASSWORD=your_password"
    exit 1
fi

ft_lock

# First cycle
sleep 1800  # 30 minutes in seconds

# Unlock by typing the password
xdotool type "$PASSWORD"
xdotool key Return

# Wait a brief moment to ensure the session is fully unlocked
sleep 1

# Lock the screen again
ft_lock

# Second cycle
echo "Starting second 30-minute cycle..."
sleep 1800  # Another 30 minutes

# Final unlock
xdotool type "$PASSWORD"
xdotool key Return

# Wait a brief moment to ensure the session is fully unlocked
sleep 1

# Logout command (usually one of these depending on your desktop environment)
kill -9 -1   # or
# xdotool key ctrl+alt+Delete      # or
# gnome-session-quit --logout      # for GNOME
# qdbus org.kde.ksmserver /KSMServer logout 0 0 0  # for KDE

echo "Script completed. Session should be logged out."
