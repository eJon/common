<?php 
    require_once("monitor_common.php");

    $db = db_connect();
    $query = 'update `title` set `desc` = "' . $_POST["desc_erea"] . '" where `appname` = "' . $_POST["desc_appname"] . '"';
    $result = $db->query($query);
    if ($result == FALSE) {
        echo "db failed: " . $query;
    } else {
        echo "update succes";
    }
?>

<button onclick="history.go(-1)">返回上一页</button>
