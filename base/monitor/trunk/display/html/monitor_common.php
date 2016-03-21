<?php
// connect to the database;
function db_connect() {
    $db = new mysqli('10.75.26.127', "root", "123456", "monitor");
    if (mysqli_connect_errno()) {
        echo "Error: Counld not connect to database. Please connect to the administrator.";
        die("can not connect to db(10.75.26.127,root,123456,monitor)"); 
    }
    return $db;
}
?>
