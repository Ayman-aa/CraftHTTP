#!/bin/bash

[ -z "$PASSWORD" ] && exit 1

# First lock-sleep-unlock cycle
(ft_lock && sleep 0.5 && xdotool type "$PASSWORD" && xdotool key Return) &
sleep 10  # 30 minutes

# Second lock-sleep-unlock cycle
(ft_lock && sleep 0.5 && xdotool type "$PASSWORD" && xdotool key Return) &
sleep 10  # 30 minutes

#kill -9 -1
