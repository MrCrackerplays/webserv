#!/usr/local/bin/python3
# import cgi, os
print ("""
   Content-Type: text/html\r\n
   <html>
   <body>
      <p>simple py test</p>
   </body>
   </html>
   """
)

# <html>
# <body>
#    <form enctype = "multipart/form-data"
#                      action = "post-upload.py" method = "post">
#    <p>File: <input type = "file" name = "filename" /></p>
#    <p><input type = "submit" value = "Upload" /></p>
#    </form>
# </body>
# </html>

# env = os.environ
# form = cgi.FieldStorage()

# # Get filename here.
# fileitem = form['filename']
# # Test if the file was uploaded
# if fileitem.filename:
#    # strip leading path from file name to avoid
#    # directory traversal attacks
#    fn = os.path.basename(fileitem.filename)
#    open(env.get('PATH_TRANSLATED', './') + fn, 'wb').write(fileitem.file.read())
#    message = 'The file "' + fn + '" was uploaded successfully'


# else:
#    message = 'No file was uploaded'
 
# print ("""
#    Content-Type: text/html\r\n
#    <html>
#    <body>
#       <p>%s</p>
#    </body>
#    </html>
#    """ % (message,)
# )
