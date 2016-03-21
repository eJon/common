#!/usr/bin/env python
import sys,os,json,time
from collections import defaultdict
import MySQLdb as mdb
local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
sys.path.append(local_path + "/./")

def tree():
    return defaultdict(tree)
def main():
    is_title = False
    last = defaultdict(int)
    while (True):
        con = None
        try:
            con = mdb.connect('10.75.26.127', 'root', '123456', 'firehose');
            con2 = mdb.connect('10.75.26.127', 'root', '123456', 'monitor');
            cur = con.cursor();
            cur2 = con2.cursor();
            values = {}
            values['lines'] = defaultdict(int)
            values['lines']['part num'] = defaultdict(int);
            cur_time = int(time.time())
            cur_time = cur_time - cur_time % 300
            for  i in range(7):
                cur.execute("select `today_sent` from `session` where username=3855001400 and partnum=%d" % i);
                rows = cur.fetchall()
                if len(rows) <=0:
                    print "can not select from sql";
                row = rows[0];
                acc_value = row[0]
                if (acc_value < last[i]):
                    last[i] = 1;
                values['lines']['part num']["part_%d" % i ]  = acc_value - last[i];
                values['lines']['total'] += acc_value - last[i]; 
                last[i] = acc_value;
            cur2.execute("insert into `kpis` (`appname`,`timestamp`,`kpi_value`) values('app-firehose', %d, '%s')" % (cur_time, json.dumps(values)))
            print json.dumps(values)
            if not is_title:
                title = tree()
                for kpi_name in values:
                    for dim_name in values[kpi_name]:
                        if (dim_name != "total"):
                            title[kpi_name][dim_name] = values[kpi_name][dim_name].keys()
                        else:
                            title[kpi_name]["NONE"] = [];
                if len(title) <= 0:
                    continue
                title["data_path"] = 'mysql';
                title_json = json.dumps(title);

                cur2.execute("select `appname` from `title` where `appname`='%s'"% 'app-firehose')
                rows = cur2.fetchall()
                if (len(rows) <= 0):
                    cur2.execute("insert into `title`(`appname`, `kpi_json`) values('%s', '%s\')" % ('app-firehose', title_json));
                else:
                    cur2.execute("update `title` set `kpi_json` = '%s' where `appname`='%s'" % (title_json,'app-firehose'))
            con.commit();
            con2.commit();
            time.sleep(300)
        except mdb.Error, e:
            print "Error %d: %s"% (e.args[0],e.args[1])
        finally:
            if con:
                con.close()
            


if __name__ == '__main__':
    main()

