#!/usr/bin/env python3

import os
import http.cookies

print("Content-Type: text/plain\n")
print("HTTP_COOKIE =", os.environ.get("HTTP_COOKIE", "None"))

# Parse cookies
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
visited = cookie.get("visited")

# If already visited, redirect to page2
if visited and visited.value == "yes":
    print("Status: 302 Found")
    print("Location: /cgi-bin/page2.py")
    print()
    exit()

# Otherwise, set cookie and show first visit page
print("Set-Cookie: visited=yes; Max-Age=3600; Path=/")
print("Content-Type: text/html\n")

print("""
<html>
<head><title>Welcome</title></head>
<body>
<h1>👋 First Visit</h1>
<p>Thanks for visiting! You'll only see this once.</p>
</body>
</html>
""")
