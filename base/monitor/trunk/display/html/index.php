<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="morris-0.4.3.min.css">

<script src="jquery.min.js"></script>
<script src="raphael-min.js"></script>
<script src="morris-0.4.3.min.js"></script>

<center><h1>微博广告KPI监控系统</h1></center>

<?php
require_once("monitor_common.php");
function get_title($db) {
    $title = array();
    $query = 'select * from `title`';
    $result = $db->query($query);
    if ($result->num_rows <= 0) {
        die("can not find any apps!");
    }
    for ($i = 0 ; $i < $result->num_rows; $i++) {
        $row = $result->fetch_assoc();
        $kpi_name = json_decode($row['kpi_json'],true);
        $title[$row['appname']] = $kpi_name;
        $title[$row['appname']]['desc'] = $row['desc'];
        $title[$row['appname']]['alert_string'] = $row['alert_string'];
    }
    return $title;
}


function get_kpi_from_sql($db,&$items, $app , $name, $dim, $dimvalues) {
    $cur_time=time();
    $cur_time = $cur_time - $cur_time % 300;

    $kpis=array();
    $todays=array();
    $yestdays=array();
    $start = $cur_time - 24 * 3600;
    $start2 = $start - 24 * 3600;
    for ($i=0;$i<24 * 12;$i++) {
        array_push($todays, $start + $i * 300);
        array_push($yestdays, $start2 + $i * 300);
    }


    sort($todays);
    sort($yestdays);
    $query = 'select * from kpis where `appname`="' . $app . '" and timestamp>=' . strval($cur_time - 3600 * 24 * 2);
    $result = $db->query($query);
    if ($result->num_rows<=0) {
        die("can not find any dat from sql " . $query);
    }
    $data = array();
    for ($i = 0; $i < $result->num_rows; $i++) {
        $row = $result->fetch_assoc();
        $data[$row['timestamp']] = json_decode($row['kpi_value'], true);
    }

    for ($i = 0 ; $i < sizeof($todays); $i++) {
        $t = $todays[$i];
        $t2= $yestdays[$i];
        $kvs=array();

        $kvs["stamp"] = $t * 1000;


        if (sizeof($items) == 1) {
            if ($dim == "NONE") {
                $kvs[$items[0]] = $data[strval($t)][$name]["total"];
                $kvs[$items[0]."-1-day-ago"] = $data[strval($t2)][$name]["total"];
            } else {
                $kvs[$items[0]] = $data[strval($t)][$name][$dim][$dimvalues[0]];
                $kvs[$items[0]."-1-day-ago"] = $data[strval($t2)][$name][$dim][$dimvalues[0]];
            }
        } else {
            for ($j=0; $j < sizeof($items); $j++) {
                $kvs[$items[$j]] = $data[strval($t)][$name][$dim][$dimvalues[$j]];
            }
        }
        array_push($kpis, $kvs);
    }
    if (sizeof($items) == 1) {
        array_push($items, $items[0]."-1-day-ago");
    }
    return $kpis;
}
?>

<?php 
date_default_timezone_set('Asia/Shanghai');


$db = db_connect();

$title = get_title($db);

$DATA_CENTRE_DIR="/home/w/share/monitor-server/data";

$app_get=$_GET['app'];
$kpi_get=$_GET['kpi'];
$dim_get=$_GET['dim'];
$dimvalues_get=$_GET['dimvalues'];

$filename=$DATA_CENTRE_DIR."/"."merge.title.v3";

if (!file_exists($filename)) {
    print "no exists ".$filename;
    exit(1);
}
$file_handle=fopen($filename, "r");
$line=fgets($file_handle);



?>

<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="get">
    <span class="app">app:
      <select name="app" id="app" >
      </select>  
    </span>  
    <span class="kpi">kpi:
      <select id="kpi" name="kpi">
      </select>  
    </span>


	<span class="dim">
	dim:
	<select id = "dim" name="dim">
	</select>
	</span>
	<span id="dimvalue" class="dimvalue"> 纬度值域: 
	</span>
    <input type="submit"  value="submit" />
    <br>
</form>

<form action="desc_submit.php" method="post">
    本监控描述：<br>
    <input id="desc_appname" type="hidden" name="desc_appname" value="ddddd" />
    <textarea id="desc_erea" name="desc_erea" rows="5" cols="100"> </textarea>
    <input type="submit" value="修改并提交"/>
</form>

<form action="alert_submit.php" method="post">
    本监控报警：<br>
    <input id="alert_appname" type="hidden" name="alert_appname" value="ddddd" />
    <textarea id="alert_erea" name="alert_erea" rows="5" cols="100"> </textarea>
    <input type="submit" value="修改并提交"/>
</form>


<span id="desc" class="desc"> </span> <br>
<script>

var title = <?php echo json_encode($title) ?>;


function appSelectChanged(app_name_select,desc_erea_id, kpi_name_id) {
    var app_name = app_name_select.val();
    kpi_name_id.options.length = 0;
    var kpi_name_array = new Array();
    var is_desc = 0;
    var desc_appname_id = document.getElementById("desc_appname");
    desc_appname_id.value = app_name;
    desc_erea_id.value = title[app_name]["desc"];
    var alert_erea_id = document.getElementById("alert_erea");
    alert_erea_id.value = title[app_name]["alert_string"];
    var alert_appname_id = document.getElementById("alert_appname");
    alert_appname_id.value = app_name;
    for (var kpi_name in title[app_name]) {
        if (kpi_name == "desc") {
            is_desc =1;
        }
        kpi_name_array.push(kpi_name);
    }
    kpi_name_array.sort();
    for (var i in kpi_name_array) {
        var kpi_name = kpi_name_array[i];
        if (kpi_name == "data_path") {
            continue;
        }
        if (kpi_name == "desc") {
            continue;
        }

        if (kpi_name == "alert_string") {
            continue;
        }
        kpi_name_id.options[kpi_name_id.length] = new Option(kpi_name, kpi_name);
        if (kpi_name == "<?php echo $kpi_get; ?>") {
            kpi_name_id.options[kpi_name_id.length-1].selected = "selected";
        }
    }
}

function kpiSelectChanged(app_name, kpi_name_select, dim_name_id) {
    var kpi_name = kpi_name_select.val();
    dim_name_id.options.length = 0;
	for (var dim_name in title[app_name][kpi_name]) {
        dim_name_id.options[dim_name_id.length] = new Option(dim_name, dim_name);
		if (dim_name == "<?php echo $dim_get ?>") {
           dim_name_id.options[dim_name_id.length-1].selected = "selected";
		}
    }		
}

function dimSelectChanged(app_name, kpi_name, dim_name, dimvalues_id) {
        var dimvalues_html = "dimvalue: ";
        for (var x in title[app_name][kpi_name][dim_name]) {
                var dim_value = title[app_name][kpi_name][dim_name][x];
                dimvalues_html += dim_value + "<input type=\"checkbox\" name=\"dimvalues[]\" value=\"" + dim_value + "\"/>    ";
        }
        dimvalues_id.innerHTML = dimvalues_html;
}



window.onload=function()
{

  var app_name_select = $(".app").children("select");

  var app_name_id = document.getElementById("app"); 
  var desc_erea_id = document.getElementById("desc_erea");
  app_name_id.options.length = 0;
  

  for (var app_name in title) {
      app_name_id.options[app_name_id.length] = new Option(app_name, app_name);
  	  if (app_name == "<?php echo $app_get; ?>") {
          app_name_id.options[app_name_id.length-1].selected = 'selected';
  	  }
  
  }

  var kpi_name_select=$(".kpi").children("select");
  var kpi_name_id = document.getElementById("kpi");


  var dim_name_select=$(".dim").children("select");
  var dim_name_id=document.getElementById("dim");

  var dimvalues_id = document.getElementById("dimvalue");


  appSelectChanged(app_name_select, desc_erea_id, kpi_name_id);
  kpiSelectChanged(app_name_select.val(), kpi_name_select, dim_name_id);
  dimSelectChanged(app_name_select.val(),kpi_name_select.val(), dim_name_select.val(), dimvalues_id);
  app_name_select.change(function() {
      appSelectChanged(app_name_select, desc_erea_id, kpi_name_id);
  });

  kpi_name_select.change(function() {
      kpiSelectChanged(app_name_select.val(), kpi_name_select, dim_name_id);
  });

  dim_name_select.change(function(){
      dimSelectChanged(app_name_select.val(),kpi_name_select.val(), dim_name_select.val(), dimvalues_id);
  });
}
</script>

<?php
// author hongbin2@staff.sina.com.cn



$name=$_GET['kpi'];


$cur_time=time();
$cur_time = $cur_time - $cur_time % 300;
$dim=$_GET['dim'];
$dimvalues=$_GET['dimvalues'];


$timestamps=array();

$filename=$title[$app_get]["data_path"];

if ($filename=='mysql') {
     
} else {

if (!file_exists($filename)) {
    print "no exists ".$filename;
    exit(1);
}
}

$kpis=array();
$items=array();
if ($dim != "NONE") {

   foreach($dimvalues as $dimvalue) {
      array_push($items, $name."-".$dim."-".$dimvalue);
   }
} else {
   array_push($items, $name);
}
if ($filename == 'mysql') {
    $kpis = get_kpi_from_sql($db, $items,$app_get, $name, $dim, $dimvalues);
} else {
$file_handle=fopen($filename, "r");
$line=fgets($file_handle);
$data=json_decode($line,True);


$todays=array();
$yestdays=array();
$start = $cur_time - 24 * 3600;
$start2 = $start - 24 * 3600;
for ($i=0;$i<24 * 12;$i++) {
	array_push($todays, $start + $i * 300);
	array_push($yestdays, $start2 + $i * 300);
}

sort($todays);
sort($yestdays);
for ($i = 0 ; $i < sizeof($todays); $i++) {
    $t = $todays[$i];
	$t2= $yestdays[$i];
    $kvs=array();

	$kvs["stamp"] = $t * 1000;
	if (sizeof($items) == 1) {
        if ($dim == "NONE") {
		    $kvs[$items[0]] = $data[strval($t)][$name]["total"];
			$kvs[$items[0]."-1-day-ago"] = $data[strval($t2)][$name]["total"];
		} else {
            $kvs[$items[0]] = $data[strval($t)][$name][$dim][$dimvalues[0]];
		    $kvs[$items[0]."-1-day-ago"] = $data[strval($t2)][$name][$dim][$dimvalues[0]];
		}
	} else {
	    for ($j=0; $j < sizeof($items); $j++) {
            $kvs[$items[$j]] = $data[strval($t)][$name][$dim][$dimvalues[$j]];
		}
	}
	array_push($kpis, $kvs);
}
if (sizeof($items) == 1) {
		array_push($items, $items[0]."-1-day-ago");
}
}
?>

<div id="myfirstchart" style="height: 400px;"></div>
<script>

var data = <?php echo json_encode($kpis) ?>;
var items= <?php echo json_encode($items) ?>;
var colors = ['#000000','#C71585', '#800000', '#8B4513', '#006400', '#2F4F4F',  '#0000CD'];
new Morris.Line({
  element: 'myfirstchart',
  data: data,
  xkey: 'stamp',
  ykeys: items,
  labels: items,
  xLabelAngle: 60,
  lineWidth:2,
  pointSize:1,
  lineColors: colors,
  pointFillColors: colors,
  pointStrokeColors: colors,
});
</script> 
