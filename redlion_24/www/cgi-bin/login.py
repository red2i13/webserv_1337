# #!/usr/bin/env python3

# import os
# import sys
# import http.cookies
# import shelve
# import time
# from urllib.parse import parse_qs

# SESSION_DB = "/tmp/session_data"
# SESSION_TIMEOUT = 600  # 10 minutes

# # ==== Read POST data if needed ====
# method = os.environ.get("REQUEST_METHOD", "")
# if method == "POST":
#     content_length = int(os.environ.get("CONTENT_LENGTH", 0))
#     post_data = sys.stdin.read(content_length)
#     form = parse_qs(post_data)
# else:
#     form = {}

# # ==== Parse cookies ====
# cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
# session_cookie = cookie.get("session_id")

# # ==== Handle logout ====
# if form.get("logout", ["0"])[0] == "1":
#     if session_cookie:
#         session_id = session_cookie.value
#         with shelve.open(SESSION_DB, writeback=True) as sessions:
#             if session_id in sessions:
#                 del sessions[session_id]
#     print("Status: 303 See Other")
#     print("Set-Cookie: session_id=; Max-Age=0; HttpOnly; Path=/")
#     print("Location: /login.py")
#     print()
#     sys.exit(0)

# # ==== No session cookie? redirect ====
# if not session_cookie:
#     print("Status: 303 See Other")
#     print("Location: /login.py")
#     print()
#     sys.exit(0)

# # ==== Session check ====
# session_id = session_cookie.value
# current_time = time.time()

# with shelve.open(SESSION_DB, writeback=True) as sessions:
#     session = sessions.get(session_id)

#     if not session:
#         print("Status: 303 See Other")
#         print("Location: /login.py")
#         print()
#         sys.exit(0)

#     if current_time - session["last_seen"] > SESSION_TIMEOUT:
#         del sessions[session_id]
#         print("Status: 303 See Other")
#         print("Set-Cookie: session_id=; Max-Age=0; HttpOnly; Path=/")
#         print("Location: /login.py")
#         print()
#         sys.exit(0)

#     session["last_seen"] = current_time
#     session["visits"] += 1
#     sessions[session_id] = session

# # ==== Output final HTML ====
# print("Content-Type: text/html")
# print(f"Set-Cookie: session_id={session_id}; Max-Age={SESSION_TIMEOUT}; HttpOnly; Path=/")
# print()
# print(f"""
# <html>
# <head><title>Dashboard</title></head>
# <body>
#     <h1>Welcome, {session['username']}!</h1>
#     <p>You have visited this dashboard {session['visits']} times.</p>
#     <p>Last active: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(session['last_seen']))}</p>
#     <form method="POST" action="/cgi-bin/dashbord.py">
#         <input type="hidden" name="logout" value="1">
#         <button type="submit">Logout</button>
#     </form>
# </body>
# </html>
# """)

#!/usr/bin/env python3

# import os
# import http.cookies
# import time
# import shelve
# import uuid

# SESSION_DB = "/tmp/session_data"
# SESSION_TIMEOUT = 600  # 10 minutes

# print("Content-Type: text/html")

# # ====== Create New Session ======
# session_id = str(uuid.uuid4())
# current_time = time.time()

# # Store the session in shelve
# with shelve.open(SESSION_DB, writeback=True) as sessions:
#     sessions[session_id] = {
#         "username": "guest",  # or real username if using login form
#         "last_seen": current_time,
#         "visits": 0
#     }

# # Set cookie
# print(f"Set-Cookie: session_id={session_id}; Max-Age={SESSION_TIMEOUT}; HttpOnly; Path=/")
# print(f"Location: /cgi-bin/dashbord.py")
# print("Status: 303 See Other")
# print()


#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb
import shelve
import uuid
import time
import http.cookies

cgitb.enable()

USER_DB = "/tmp/user_data"
SESSION_DB = "/tmp/session_data"
SESSION_TIMEOUT = 600  # 10 minutes

form = cgi.FieldStorage()
method = os.environ.get("REQUEST_METHOD", "GET")

# ==== Handle POST: Login Attempt ====
if method == "POST":
    username = form.getfirst("username", "").strip()
    password = form.getfirst("password", "").strip()

    with shelve.open(USER_DB) as users:
        if username in users and users[username] == password:
            # Login successful — create session
            session_id = str(uuid.uuid4())
            current_time = time.time()
            with shelve.open(SESSION_DB, writeback=True) as sessions:
                sessions[session_id] = {
                    "username": username,
                    "last_seen": current_time,
                    "visits": 0
                }

            # Set session cookie and redirect
            print(f"Set-Cookie: session_id={session_id}; Max-Age={SESSION_TIMEOUT}; HttpOnly; Path=/")
            print("Status: 303 See Other")
            print("Location: /cgi-bin/dashbord.py")
            print()
            sys.exit(0)
        else:
            # Invalid credentials
            print("Content-Type: text/html")
            print()
            print("<p style='color:red;'>Invalid username or password.</p>")

# ==== Show Login Form ====
# print("Content-Type: text/html")
print()
print("""
<html>
<head><title>Login</title></head>
<body>
<h2>Login</h2>
<form method="POST" action="/cgi-bin/login.py">
    <label>Username: <input type="text" name="username"></label><br/>
    <label>Password: <input type="password" name="password"></label><br/>
    <input type="submit" value="Login">
</form>
<p>Don't have an account? <a href="/cgi-bin/register.py">Register here</a></p>
</body>
</html>
""")

