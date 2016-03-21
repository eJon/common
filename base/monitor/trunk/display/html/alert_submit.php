<?php 
    require_once("monitor_common.php");

    $db = db_connect();
    $query = 'update `title` set `alert_string` = ' . "'" .  $_POST["alert_erea"] . "'". ' where `appname` = "' . $_POST["alert_appname"] . '"';
    print $query;
    $result = $db->query($query);
    if ($result == FALSE) {
        echo "db failed: " . $query;
    } else {
        echo "update succes";
    }
?>

<button onclick="history.go(-1)">返回上一页</button>
