#!/usr/bin/env python
import sys,os,json,time,re
from collections import defaultdict
import MySQLdb as mdb
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")
from common_utils import KpiColTimeline


class IndexserverlogParser():
    def run(self):
        app = "bidfeed-indexserverlog"
        values = self.extract_kpi()
        print json.dumps(values.to_dict())
        try:
            con = mdb.connect('10.75.26.127', 'root', '123456','monitor');
            cur = con.cursor(); 
            values.calc_total()
            #values.calc_rate()
            for timestamp in values.cols:
                print json.dumps(values.cols[timestamp].to_dict())
                cur.execute("select `appname` from `kpis` where `appname`='%s' and `timestamp`='%s'" % (app, timestamp))
                rows = cur.fetchall()
                if len(rows) <= 0:
                    sql = "insert into `kpis`(`appname`,`timestamp`,`kpi_value`) values('%s',%d,'%s')"%(app, timestamp, json.dumps(values.cols[timestamp].to_dict()))
                    print sql
                    cur.execute(sql)
                else:
                    sql = "update `kpis` set `kpi_value` = '%s' where `appname`='%s' and timestamp='%d'" % (json.dumps(values.cols[timestamp].to_dict()), app, timestamp)
                    print sql
                    cur.execute(sql)
                con.commit()
        except mdb.Error, e:
            print "Error %d: %s"% (e.args[0],e.args[1])
            sys.exit(1)
        finally:
            if con:
                con.close()

    def extract_kpi(self):
        now = int(time.time())
        now = now - now % 300 - 300

        rootdir = os.path.join(local_path, "..")
        kpis = KpiColTimeline()
        for filename in os.listdir(rootdir):
            filepath = os.path.join(rootdir, filename)
            m = re.match(r"indexserver\.log\." + str(now) + r"\.(\d+\.\d+\.\d+\.\d+)", filename)
            if not m:
                continue
            print filepath
            for line in open(filepath):
                tokens = line.split()
                datestr = tokens[0] + " " + tokens[1]
                datestamp = int(time.mktime(time.strptime(datestr, "%Y-%m-%d %H:%M:%S")))
                #if datestamp >= now + 300:
                #    continue
                if re.search(r"ERROR", line) == None:
                    continue
                print line
                datestamp = datestamp - datestamp % 300
                kpiname = tokens[4]
                kpis.acc(datestamp, kpiname, "ip", m.group(1), 1)
        return kpis
            
    
if __name__ == '__main__':
    IndexserverlogParser().run()

