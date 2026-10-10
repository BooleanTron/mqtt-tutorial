#!/usr/bin/env bash
# Simulates the ESP32: publishes random temperature/humidity JSON every 3 seconds.
set -euo pipefail

BROKER="${1:-localhost}"
PORT="${2:-1883}"
TOPIC="test/publish"

echo "Publishing to ${BROKER}:${PORT} on topic '${TOPIC}' every 3s. Ctrl+C to stop."

while true; do
  temp_tenths=$(( 200 + RANDOM % 151 ))   # 20.0 - 35.0
  humidity_tenths=$(( 300 + RANDOM % 501 )) # 30.0 - 80.0
  temperature=$(awk -v v="$temp_tenths" 'BEGIN{printf "%.1f", v/10}')
  humidity=$(awk -v v="$humidity_tenths" 'BEGIN{printf "%.1f", v/10}')
  payload="{\"temperature\":${temperature},\"humidity\":${humidity}}"

  mosquitto_pub -h "$BROKER" -p "$PORT" -t "$TOPIC" -m "$payload"
  echo "Published: $payload"

  sleep 3
done
