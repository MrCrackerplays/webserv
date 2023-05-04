#!/usr/bin/env python3
import cgi, os, sys

env = os.environ
form = cgi.FieldStorage()

# Get filename here.
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
    # strip leading path from file name to avoid directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    save_path = env.get('SAVE_LOCATION', env.get('PATH_TRANSLATED', './'))
    with open(os.path.join(save_path, fn), 'wb') as f:
        # Read the contents of the file from stdin
        while True:
            chunk = sys.stdin.buffer.read(1024)
            if not chunk:
                break
            f.write(chunk)
    message = 'The file "' + fn + '" was uploaded successfully to ' + save_path
else:
    message = 'No file was uploaded'

print("""
Content-Type: text/html\r\n\r\n
<html>
<body>
    <p>%s</p>
</body>
</html>
""" % (message,))
