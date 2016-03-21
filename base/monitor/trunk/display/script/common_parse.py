#!/usr/bin/env python
# auther hongbin2@staff.sina.com.cn

import sys,os,re,json,time
from collections import defaultdict
import MySQLdb as mdb
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")

from common_utils import *

DATA_CENTRE="/home/w/share/monitor-server/data"
pt_ipfile=re.compile(r'^\d+\.\d+\.\d+\.\d+$')

def extract_kpi(f, kpi_col_timeline):#{{{
    for line in open(f):
        js = json.loads(line)
        content = js["content"]
        if len(content) == 0:
            continue
        ip = js["ip"]
        time_str = js["time"]
        timestamp = time.mktime(time.strptime(time_str, "%Y%m%d%H%M%S"))
        timestamp = int(timestamp - timestamp % 300)
        kpi_col = KpiCollector()
        for t in content.split("|"):
            if len(t)  == 0 :
                continue
            key = t.split(":")[0]; value = t.split(":")[1]
            kpi = KpiEntity(key)
            dim = DimEntity("ip")
            dim.acc(ip, int(value))
            kpi.add_dim(dim)
            kpi.set_total(dim.get_total())
            kpi_col.add_kpi(kpi)
        kpi_col_timeline.acc_col(timestamp, kpi_col)#}}}

def get_apps():#{{{
    result = []
    for filename in os.listdir(DATA_CENTRE):
        path = os.path.join(DATA_CENTRE, filename)
        if os.path.isdir(path):
            if os.path.isfile(os.path.join(path, 'snapshot.log')):
                result.append(filename)
    return result#}}}

def main():
    for app in get_apps():
        try:
            con = mdb.connect('10.75.26.127', 'root', '123456','monitor');
            cur = con.cursor();
            datapath = os.path.join(DATA_CENTRE, app);
            values = KpiColTimeline()
            for f in os.listdir(datapath):
                if pt_ipfile.match(f):
                    extract_kpi(os.path.join(datapath,f), values)
                    print "done %s %s"% (app, f);

            for timestamp in values.cols:
                cur.execute("select `appname` from `kpis` where `appname`='%s' and `timestamp`='%s'" % (app, timestamp)) 
                rows = cur.fetchall()
                if len(rows) <= 0:
                    cur.execute("insert into `kpis`(`appname`,`timestamp`,`kpi_value`) values('%s',%d,'%s')"%(app, timestamp, json.dumps(values.cols[timestamp].to_dict())))
                else:
                    cur.execute("update `kpis` set `kpi_value` = '%s' where `appname`='%s' and timestamp='%d'" % (json.dumps(values.cols[timestamp].to_dict()), app, timestamp))
            #title = values.gen_title()
            #if len(title) <= 0:
            #    continue
            #title["data_path"] = 'mysql';
            #title_json = json.dumps(title);

            #cur.execute("select `appname` from `title` where `appname`='%s'"% app)
            #rows = cur.fetchall()
            #if (len(rows) <= 0):
            #    cur.execute("insert into `title`(`appname`, `kpi_json`) values('%s', '%s\')" % (app, title_json));
            #else:
            #    cur.execute("update `title` set `kpi_json` = '%s' where `appname`='%s'" % (title_json,app))
            con.commit()
        except mdb.Error, e:
            print "Error %d: %s"% (e.args[0],e.args[1])
            sys.exit(1)
        finally:
            if con:
                con.close()


def set_title(cur, title, appname):
    """
    cur the db cur
    title the title of json format
    appname the appname of this title

    first check if there is already the appname in the db
    if exists, update it 
    if not, insert it
    """

    # check if there is already the appname in the db
    sql = "select `appname` from `title` where `appname`='%s'" % appname 
    cur.execute(sql)
    rows = cur.fetchall()
    if len(rows) <= 0:   # not exists
        sql = "insert into `title`(`appname`, `kpi_json`) values('%s','%s')" % (appname, title)
        cur.execute(sql)
    else:               # exists
        sql = "update `title` set `kpi_json` = '%s' where `appname`='%s'" % (title, appname)
        print sql
        cur.execute(sql)
    
def merge():
    try:
        con = mdb.connect('10.75.26.127', 'root', '123456','monitor');
        cur = con.cursor();
        cur_time = int(time.time())
        to_time = cur_time - cur_time % 5 * 60 +  5 * 60
        from_time = cur_time - 3600 * 24 * 7
        cur.execute("select `appname` from `kpis` where `timestamp`>=%d and `timestamp`<%d" % (from_time, to_time)) 
        rows = cur.fetchall()
        apps = set()
        for row in rows:
            apps.add(row[0])
        for app in apps:
            cur.execute("select `timestamp` from `kpis` where `appname`='%s' and `timestamp` >=%d and `timestamp`<%d" % (app, from_time, to_time))
            rows = cur.fetchall()
            timestamps = set()
            for row in rows:
                timestamps.add(row[0])
            kpi_col_all = KpiCollector()
            for timestamp in timestamps:
                cur.execute("select `kpi_value` from `kpis` where `appname`='%s' and `timestamp`=%d" % (app, timestamp))
                rows = cur.fetchall()
                if len(rows) > 1:
                    kpi_col = KpiCollector()
                    for row in rows:
                        kpi_value = row[0]
                        kpi_col2 = KpiCollector()
                        kpi_col2.from_dict(json.loads(kpi_value))
                        kpi_col.merge(kpi_col2)
                        cur.execute("delete from `kpis` where `appname`='%s' and `timestamp`=%d and `kpi_value`='%s'" % (app, timestamp, row[0]))
                    cur.execute("insert into `kpis`(`appname`,`timestamp`,`kpi_value`) values('%s',%d,'%s')" % (app, timestamp, json.dumps(kpi_col.to_dict())))
                    #title = json.dumps(kpi_col.gen_title())
                    #set_title(cur, title, app)
                    kpi_col_all.merge(kpi_col)
                elif len(rows) == 1: 
                    kpi_col = KpiCollector()
                    try:
                        kpi_col.from_dict(json.loads(rows[0][0]))
                        kpi_col_all.merge(kpi_col)
                    except Exception, ex:
                        print "can not parse %s " % rows[0][0]
                        print Exception, ex
            title = json.dumps(kpi_col_all.gen_title())
            print app, title
            set_title(cur, title, app)
                    


    except mdb.Error, e:
        print "Error %d: %s" % (e.args[0], e.args[1])
        sys.exit(1)
if __name__ == '__main__':
    #main()
    merge()

