<?php

# https://www.php.net/manual/fr/features.file-upload.post-method.php

$uploaddir = '../upload';

if (isset($_FILES['userfile']))
{
	$uploadfile = $uploaddir . basename($_FILES['userfile']['name']);

	if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile))
		echo "file successfully uploaded";
	else
		echo "file couldn't be uploaded";
}