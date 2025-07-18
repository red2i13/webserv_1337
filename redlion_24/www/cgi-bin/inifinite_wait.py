#!/usr/bin/env python3

import time
import sys

# --- CGI Header ---
# This is crucial for the web server to understand the response.
# We're sending plain text.
print("Content-Type: text/plain")
print() # A blank line is required after headers

print("CGI script started. This script will run indefinitely to test server timeout.")
print("Waiting for server to timeout...")
sys.stdout.flush() # Ensure this initial message is sent to the client immediately

# --- Infinite Loop ---
# This loop will run forever, simulating a process that never finishes.
# The 'time.sleep()' is added to prevent it from consuming 100% CPU,
# while still ensuring it doesn't return a response.
try:
    while True:
        # You can add a small delay to prevent excessive CPU usage,
        # but keep it running indefinitely.
        time.sleep(1)
        # Optionally, print something to the server logs or a debug file
        # to confirm it's still running, but don't send it to stdout
        # unless you want to see partial output before timeout.
        # print("Still running...", file=sys.stderr) # This writes to server's error log
except Exception as e:
    # This block might catch exceptions if the server forcibly terminates the script,
    # but often, the script is just killed without Python's exception handling.
    print(f"Script terminated unexpectedly: {e}", file=sys.stderr)
finally:
    # This block might not always execute if the script is forcibly killed.
    print("CGI script finished (unexpectedly, if timeout occurred).", file=sys.stderr)

