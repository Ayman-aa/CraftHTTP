#!/bin/bash

while true; do
    # Move the mouse cursor slightly
    xdotool mousemove_relative 5 0
    xdotool mousemove_relative -5 0
    
    # Wait for 1 minute before moving the mouse again
    sleep 6
done
