<?php

    require_once("config.php");

    function Connect($host, $user, $passwd, &$link) {
		$link = mysql_connect($host, $user, $passwd);
		if (!$link) {
			die('Could not connect: ' . mysql_error());
		}
	
	    $db_selected = mysql_select_db('cpm', $link);
	    if (!$db_selected) {
	        die ('Can\'t use cpm : ' . mysql_error());
	    }
    }

    function MysqlQuery($query, $errmsg, &$result) {
        $result = mysql_query($query);
        if (!$result) {
            die('mysql_query :' . $errmsg . " : " . mysql_error());
        }
    }

    function Fetch(&$result, &$ret_array) {
        $ret_array = array();
        while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
            array_push($ret_array, $row);
        }

        mysql_free_result($result); 
    }

    function TairQuery($tool, $host, $group, $area, $pdps, $adid, &$value) {
        $tair_adid = $adid . date('Y-m-d');
        $query = sprintf("%s %s %s %s %s", $tool, $host, $group, $area, $tair_adid);
        exec($query, $output);
        //var_dump($output);
        if (strstr($output[0], "get value:") != FALSE) {
            $value = substr($output[0], strpos($output[0], ":") + 1);
            //var_dump("tair value:".$value);
            return true;
        }

        $alert_msg = "PDPS:$pdps Adid:$adid is out of tair";
        Alert($alert_msg, $alert_tool, $alert_sv, $alert_service, $alert_object, $alert_subject, $alert_msgto, $alert_mailto);
        var_dump($alert_msg);
        return false;
    }

    function Alert(&$alert_msg, $tool, $sv, $service, $object, $subject, $msgto_list, $mailto_list) {
        system("$tool --sv $sv --service $service --object $object --subject $subject --content \"$alert_msg\" --msgto $msgto_list --mailto $mailto_list > /dev/null 2>&1"); 
    }

    function Monitor(&$tool, $key, $value) {
        system("$tool $key $value > /dev/null 2>&1"); 
        //echo "$tool $key $value > /dev/null 2>&1\n"; 
        return true;
    }

    Connect($mysql_host, $mysql_user, $mysql_passwd, $link);

    // PDPS000000046737,PDPS000000046736,PDPS000000037693为废弃广告位
    // PDPS000000052321为wireless的pdps, 只报警没投放
    // 其余为web的pdps，需要报没投放和投放不足(阈值由配置文件设置，可调整)
    $query = 'select distinct pdps from `order` where DATEDIFF(end_time, current_timestamp()) > 0 and LENGTH(pdps) > 0 and pdps != \'PDPS000000046737\' and pdps != \'PDPS000000046736\' and pdps != \'PDPS000000037693\'';
    //var_dump($query);
    MysqlQuery($query, "select distinct pdps from order", $result);

    Fetch($result, $pdps_array);
    foreach ($pdps_array as $value) {
        $pdps = $value['pdps'];
        var_dump("pdps: ".$pdps);

        $pdps_reserve = 0;
        $pdps_consume = 0;

        // impression = 99999999为抄底广告, 不监控
        $query = sprintf("select ad_id from ad_cpm_info_t_sina_com where impression != 99999999 and psid = '%s' and DATEDIFF(end_date, current_timestamp()) >= 0", mysql_real_escape_string($pdps));
        //var_dump($query);

        MysqlQuery($query, "select id from order", $result);
        Fetch($result, $adid_array);
        foreach ($adid_array as $val) {
            usleep($query_interval_in_millisecond * 1000);

            $adid = $val['ad_id'];
            //var_dump("adid:".$adid);
            
            if (!TairQuery($tair_tool, $tair_host, $tair_grop, $tair_area, $pdps, $adid, $tair_value)) {
                continue;
            } 

            $adid_reserve = 0;
            $adid_consume = 0;

            $query = sprintf("select impression, impressioned from order_impressions where order_id = %s and DATEDIFF(post_date, current_date()) = 0", $adid);
            //var_dump($query);
            MysqlQuery($query, "select impression, impressioned from order_impressions", $result);
            Fetch($result, $imp_array);
            if (empty($imp_array)) {
                $alert_msg = "PDPS:$pdps Adid:$adid has no impression data in MySQL";
                Alert($alert_msg, $alert_tool, $alert_sv, $alert_service, $alert_object, $alert_subject, $alert_msgto, $alert_mailto);
                var_dump($alert_msg);
                continue;
            } 

            foreach ($imp_array as $imp_val) { 
                $impression = $imp_val["impression"]; 
                $impressioned = $imp_val["impressioned"]; 
                $imp_diff = $impression - $impressioned;
            
                $adid_reserve += $impression;
                $adid_consume += $impressioned;
                if ($adid_reserve != 0) {
                    $rate = $adid_consume / $adid_reserve;
                    system("$addkey_tool $adid > /dev/null 2>&1");
                    Monitor($monitor_tool, $adid, $rate);
                }

                $pdps_reserve += $impression;
                $pdps_consume += $impressioned;

                if (strcmp($pdps, $wireless_pdps) == 0 && $impressioned < $wireless_alert_threshold) {
                    //无线端只在无投放时报警
                    $alert_msg = "PDPS:$pdps Adid:$adid impressioned:0 impression:$impression tair_value:$tair_value";
                    Alert($alert_msg, $alert_tool, $alert_sv, $alert_service, $alert_object, $alert_subject, $alert_msgto, $alert_mailto);
                }
                else if (abs($imp_diff - $tair_value) > $web_alert_threshold) { 
                    $alert_msg = "PDPS:$pdps Adid:$adid impressioned:$impressioned impression:$impression imp_diff:$imp_diff tair_value:$tair_value"; 
                    Alert($alert_msg, $alert_tool, $alert_sv, $alert_service, $alert_object, $alert_subject, $alert_msgto, $alert_mailto); 
                    var_dump($alert_msg);
                }
            }
        }

        if ($pdps_reserve != 0) {
            $rate = $pdps_consume / $pdps_reserve;
            system("$addkey_tool $pdps > /dev/null 2>&1");
            Monitor($monitor_tool, $pdps, $rate);
        }
    }
    
	mysql_close($link);
?>
