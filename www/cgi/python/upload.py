import cgi

form = cgi.FieldStorage()

# Default message error if not even try
message = "file not found"

# Get filename here
file = form["file"]

#If file uploaded
if file.file:
	#Wb means write & binary, is a filemod of open
	with open(f"www/cgi/upload/{file.filename}", 'wb') as f:
		try:
			f.write(file.file.read())
		except Exception as e:
			message = 'file couldn\'t be uploaded'
		else:
			message = 'file successfully uploaded'

print("Content-type: text/html\r\n\r\n")
print(message)  