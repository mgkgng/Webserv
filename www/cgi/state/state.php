<?php

# Session_start() creates a session or resumes the current one based on a session identifier passed via a GET or POST request, or passed via a cookie.
# In this case, being a test, we will recall (if succesfull) the session started by index.php
session_start();

# https://www.php.net/manual/en/reserved.variables.session.php
# Basically, if there is the reserved variable session, we will echo a test and two br (newline, in html)
if (isset($_SESSION["eval_test"]))
	echo $_SESSION["eval_test"];
    echo "<br><br>";

# https://www.php.net/manual/fr/reserved.variables.cookies.php
# Same as before, if the reserved variable cookie is set, we echo it to show it works
if (isset($_COOKIE["eval_test"]))
	echo "Cookie are succesfully working!<br>";