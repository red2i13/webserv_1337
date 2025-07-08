#!/usr/bin/env python3

import cgi
import os

print("Content-Type: text/html\n")

print("<html><body><h1>Upload a File</h1>")

form = cgi.FieldStorage()

# Debug: print form keys
print(f"Form keys: {list(form.keys())}<br>")

if os.environ.get("REQUEST_METHOD", "") == "POST":
    if "file" in form:
        print("Processing uploaded file...<br>")
        fileitem = form["file"]
        if fileitem.filename:
            filename = os.path.basename(fileitem.filename)
            upload_dir = "./www/uploads/"
            save_path = os.path.join(upload_dir, filename)
            # Ensure upload directory exists
            os.makedirs(upload_dir, exist_ok=True)
            with open(save_path, "wb") as f:
                f.write(fileitem.file.read())
            print(f"<p>File <b>{filename}</b> uploaded successfully!</p>")
        else:
            print("<p>No file was selected.</p>")
    else:
        print("<p>No file field in the form.</p>")


print("""
<form enctype="multipart/form-data" method="post">
    <input type="file" name="file">
    <input type="submit" value="Upload">
</form>
</body></html>
""")