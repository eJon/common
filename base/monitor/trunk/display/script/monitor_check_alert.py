#!/usr/bin/env python
# -- coding: gbk --
# auther hongbin2@staff.sina.com.cn

import sys,os,re,json,time,urllib
import requests
from collections import defaultdict
import MySQLdb as mdb
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")

from common_utils import *

DATA_CENTRE="/home/w/share/monitor-server/data"
pt_ipfile=re.compile(r'^\d+\.\d+\.\d+\.\d+$')

def get_alerts(cur):
    alerts = {}
    query = "select `appname`, `alert_string` from `title` where `alert_string` is not null"
    cur.execute(query)
    rows = cur.fetchall()
    for row in rows:
        alerts[row[0]] = row[1]
    return alerts

def alert(mail, phone):
    data = {}
    data['user'] = mail
    data['coding'] = 'gbk'
    data['phones'] = phone
    data['emails'] = mail + "@staff.sina.com.cn"
    data['email_title'] = alert_title
    data['email_content'] = data['sms_content'] = alert_content + "<br> 如果需要查看, 修改, 订阅 请访问: http://dashboard.biz.weibo.com/index.php?app=" + alert_appname 
    url = 'http://20.pub.sina.com.cn:18080/add_email_sms'
    rst = requests.post(url, data)
    if not rst.ok and rst.content.count("'success':'1'") != 1:
        raise Exception("send_sms() failed")
    print rst.content

def main():
    try:
        con = mdb.connect('10.75.26.127', 'root', '123456','monitor');
        cur = con.cursor();
        cur_time = int(time.time())
        to_time = cur_time - cur_time % 5 * 60 +  5 * 60
        from_time = cur_time - 5 * 60 * 4
        cur.execute("select `appname` from `kpis` where `timestamp`>=%d and `timestamp`<%d" % (from_time, to_time)) 
        rows = cur.fetchall()
        apps = set()
        for row in rows:
            apps.add(row[0])
        alerts = get_alerts(cur)
        print json.dumps(alerts)
        for app in apps:
            print "."
            if not app in alerts:
                print app
                continue
            print app
            cur.execute("select `timestamp` from `kpis` where `appname`='%s' and `timestamp` >=%d and `timestamp`<%d" % (app, from_time, to_time))
            rows = cur.fetchall()
            timestamps = set()
            for row in rows:
                timestamps.add(row[0])
            for timestamp in timestamps:
                print ".",
                query = "select `kpi_value` from `kpis` where `appname`='%s' and `timestamp`=%d" % (app, timestamp)
                print query
                cur.execute(query)
                rows = cur.fetchall()
                if len(rows) > 1:
                    kpi_col = KpiCollector()
                    for row in rows:
                        kpi_value = row[0]
                        kpi_col2 = KpiCollector()
                        kpi_col2.from_dict(json.loads(kpi_value))
                        kpi_col.merge(kpi_col2)
                        cur.execute("delete from `kpis` where `appname`='%s' and `timestamp`=%d and `kpi_value`='%s'" % (app, timestamp, row[0]))
                    values = kpi_col.to_dict()
                elif len(rows) == 1: 
                    values = json.loads(rows[0][0])
                print json.dumps(values)
                global alert_title
                alert_title= "微博广告KPI监控系统报警-" + app
                global alert_content 
                global alert_appname 
                alert_appname = app
                alert_content= alerts[app]
                exec_str = alerts[app].replace("\r\n","\n")
                print exec_str
                exec(exec_str)
                print "exec done"
                    


    except mdb.Error, e:
        print "Error %d: %s" % (e.args[0], e.args[1])
        sys.exit(1)
if __name__ == '__main__':
    main()

