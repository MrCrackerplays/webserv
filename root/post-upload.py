#!/usr/local/bin/python3
import cgi, os
import sys


# <html>
# <body>
#    <form enctype = "multipart/form-data"
#                      action = "post-upload.py" method = "post">
#    <p>File: <input type = "file" name = "filename" /></p>
#    <p><input type = "submit" value = "Upload" /></p>
#    </form>
# </body>
# </html>

env = os.environ

form = cgi.FieldStorage()

# Get filename here.
fileitem = form['filename']
# Test if the file was uploaded
if fileitem.filename:
   # strip leading path from file name to avoid
   # directory traversal attacks
   fn = os.path.basename(fileitem.filename)
   open(env.get('SAVE_LOCATION', env.get('PATH_TRANSLATED', './')) + fn, 'wb').write(fileitem.file.read())
   message = 'The file "' + fn + '" was uploaded successfully to ' + env.get('SAVE_LOCATION', env.get('PATH_TRANSLATED', './')) + fn


else:
   message = 'No file was uploaded'
 
print ("""
   Content-Type: text/html\r\n\r\n
   <html>
   <body>
      <p>%s</p>
   </body>
   </html>
   """ % (message,)
)
