#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")  # Set plain text for easier reading

print("=== CGI ENVIRONMENT VARIABLES ===\n")
for key, value in os.environ.items():
    print(f"{key} = {value}")
