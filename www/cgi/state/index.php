<?php

# https://www.php.net/manual/en/function.session-start.php
# With start, here we initialize a new session. As the subject ask, we will initialise a dummy var (eval_test)
session_start();

# https://www.php.net/manual/fr/function.setcookie.php
# 
$_SESSION["eval_test"] = rand();
setcookie("eval_test", "it works!", time()+3600);
?>

<!-- With an href, we redirect to state.php which will be test asked by the subject -->
<a href="./state.php">Let's test if that worked!</a>