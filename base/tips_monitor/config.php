<?php

// mysql -h m4309i.eos.grid.sina.com.cn -uengine -p'f3u4w8n7b3h' -P4309 -Dcpm
$mysql_host = "m4309i.eos.grid.sina.com.cn:4309";
$mysql_user = "engine";
$mysql_passwd = "f3u4w8n7b3h";
$mysql_database = "cpm";

// /usr/local/nginx/logs/getTairImpress 10.75.26.29:5198 group_1 0 3443212014-04-23
$tair_tool = "/usr/local/nginx/logs/getTairImpress";
$tair_host = "10.75.26.29:5198";
$tair_grop = "group_1";
$tair_area = "0";

/* 
/home/w/share/monitor/send_alert.pl --sv 微博广告 --service Ad_Engine --object tips --subject  Impression_Alert --content "Alert Message" --msgto jianqing --mailto jianqing 
*/
$alert_tool = "/home/w/share/monitor/send_alert.pl";
$alert_sv = "微博广告";
$alert_service = "Ad_Engine";
$alert_object = "tips";
$alert_subject = "Impression_Alert";
$alert_msgto = "jianqing";
//$alert_mailto = "jianqing";
$alert_mailto = "jianqing,yanyan10,dongsheng4";

$query_interval_in_millisecond = 100;

$wireless_pdps = "PDPS000000052321";

$wireless_alert_threshold = 10000;
$web_alert_threshold = 500000;

$monitor_tool = "python26 /home/w/share/monitor-display/script/kpi_tool.py set reserve_consume tips_reserve_consume";
$addkey_tool = "python26 /home/w/share/monitor-display/script/kpi_tool.py dim reserve_consume tips_reserve_consume"

?>
