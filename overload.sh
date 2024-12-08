#!/bin/bash

# URL of the server to test
URL="http://127.0.0.1:8081/movie.mp4"

# Number of requests to send
NUM_REQUESTS=100

# Loop to send requests
for i in $(seq 1 $NUM_REQUESTS); do
    curl -s -o /dev/null -w "%{http_code}\n" $URL &
done

# Wait for all background jobs to finish
# wait

echo "Sent $NUM_REQUESTS requests to $URL"