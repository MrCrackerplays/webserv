#!/usr/bin/env python3
import cgi, os


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
   dirpath = env.get('PATH_TRANSLATED', './')
   target = dirpath + fn
   savloc = env.get('SAVE_LOCATION')
   if savloc and len(savloc) > 0:
      target = savloc + fn
      dirpath = savloc
   if not os.path.exists(dirpath):
      os.makedirs(dirpath)
   open(target, 'wb').write(fileitem.file.read())
   message = 'The file "' + fn + '" was uploaded successfully to <a href="' + target + '">' + target + '</a>'


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
