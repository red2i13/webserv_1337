#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb
import shelve

cgitb.enable()

USER_DB = "/tmp/user_data"

# print("Content-Type: text/html")

form = cgi.FieldStorage()
method = os.environ.get("REQUEST_METHOD", "GET")

print()

# Handle registration POST
if method == "POST":
    username = form.getfirst("username", "").strip()
    password = form.getfirst("password", "").strip()

    if not username or not password:
        print("<p style='color:red;'>Both username and password are required.</p>")
    else:
        with shelve.open(USER_DB, writeback=True) as users:
            if username in users:
                print("<p style='color:red;'>Username already exists. Try another.</p>")
            else:
                users[username] = password
                # Redirect to login page
                print("Status: 303 See Other")
                print("Location: /cgi-bin/login.py")
                print()
                sys.exit(0)

# Show registration form
print("""
<html>
<head><title>Register</title></head>
<body>
<h2>Register</h2>
<form method="POST" action="/cgi-bin/register.py">
    <label>Username: <input type="text" name="username"></label><br/>
    <label>Password: <input type="password" name="password"></label><br/>
    <input type="submit" value="Register">
</form>
<p>Already have an account? <a href="/cgi-bin/login.py">Login here</a></p>
</body>
</html>
""")
