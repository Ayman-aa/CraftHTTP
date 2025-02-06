<?php
session_start();
if (isset($_SESSION['counter']))
    $_SESSION['counter']++;
else
    $_SESSION['counter'] = 1;
echo "Session counter: " . $_SESSION['counter'];
?>