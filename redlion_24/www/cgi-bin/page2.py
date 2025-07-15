#!/usr/bin/env python3

import os
import http.cookies

cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
visited = cookie.get("visited")

print("Content-Type: text/html\n")

if visited and visited.value == "yes":
    print("""
    <html>
    <head><title>Welcome Back</title></head>
    <body>
    <h1>🔁 Welcome Back!</h1>
    <p>This is your second (or more) visit. You won't see the first page again.</p>
    </body>
    </html>
    """)
else:
    print("""
    <html>
    <head><title>Oops</title></head>
    <body>
    <h1>⚠️ No Cookie Found</h1>
    <p>Please visit <a href="/cgi-bin/page1.py">page1</a> first to set the cookie.</p>
    </body>
    </html>
    """)
