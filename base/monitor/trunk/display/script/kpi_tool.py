#!/usr/bin/env python
# auther hongbin2@staff.sina.com.cn

import sys, os, re, json, time
import MySQLdb as mdb

def usage():
    print """
    kpi_tool.py <commond>
    <commond>:
    show <appname>
    add  <appname>
    """
def show(appname):
    try:
        con = mdb.connect("10.75.26.127", "root", '123456', 'monitor')
        cur = con.cursor()
        sql = "select `kpis` from `title` where `appname`=\"%s\"" % (appname)
        print sql
        cur.execute(sql)
        rows = cur.fetchall()
        if len(rows) == 0:
            print "there is no app named " + appname
            return
        lkpis = json.loads(rows[0][0])
        for kpi in lkpis:
            print kpi
            sql = "select `dims` from `dim` where `appname`=\"%s\" and `kpiname`='%s'" % (appname, kpi)
            cur.execute(sql)
            rows = cur.fetchall()
            if len(rows) > 0:
                for dim in json.loads(rows[0][0]):
                    print "\t" + dim
            else:
                print "\t--- no dimensions ---"
            
    except mdb.Error, e:
        print "Error %d: %s" % (e.args[0], e.args[1])

def new(appname, interval, desc):
    try:
        con = mdb.connect("10.75.26.127", "root", "123456", "monitor")
        cur = con.cursor()
        sql = "select `appname` from `title` where `appname`=\"%s\"" % (appname)
        cur.execute(sql)
        rows = cur.fetchall()
        if len(rows) > 0:
            print "there is already app named " + appname
            return

        sql = "insert into `title`(`appname`, `kpi_json`, `interval`, `version`, `desc`) values('%s', '%s', %d, %d, '%s')" % (appname, """{"data_path": "mysql"}""", interval, 301, desc)
        print sql
        cur.execute(sql)
    except mdb.Error, e:
        print "Error %d : %s" % (e.args[0], e.args[1])
def kpi(appname, kpiname):
    try:
        con = mdb.connect("10.75.26.127", "root", "123456", "monitor")
        cur = con.cursor()
        sql = "select `appname` from `title` where `appname`=\"%s\"" % (appname)
        cur.execute(sql)
        rows = cur.fetchall()
        if len(rows) == 0:
            print "there is no app named " + appname
            return

        sql = "select `kpis` from `title` where `appname`=\"%s\"" % (appname)
        cur.execute(sql)
        rows = cur.fetchall()
        jkpis = rows[0][0]
        if (jkpis == None) : jkpis = "[]"
        lkpis = set(json.loads(jkpis))
        lkpis.add(kpiname)
        jkpis = json.dumps(list(lkpis))
        sql = "update `title` set `kpis` = '%s' where `appname`='%s'" % (jkpis, appname)
        cur.execute(sql)
    except mdb.Error, e:
        print "Error %d : %s" % (e.args[0], e.args[1])
def dim(appname, kpiname, dimname):
    try:
        con = mdb.connect("10.75.26.127", "root", "123456", "monitor")
        cur = con.cursor()
        sql = "select `dims` from `dim` where `appname`=\"%s\" and `kpiname`='%s'" % (appname, kpiname)
        cur.execute(sql)
        rows = cur.fetchall()
        if len(rows) == 0:
            sql = "insert into `dim` (`appname`, `kpiname`, `dims`) values ('%s', '%s', '%s')" % (appname, kpiname, "[]")
            cur.execute(sql)
        try:
            jdims = rows[0][0]
        except IndexError, e:
            jdims = "[]"
        if (jdims == None) : jdim = "[]"
        ldims = set(json.loads(jdims))
        ldims.add(dimname)
        jdims = json.dumps(list(ldims))
        sql = "update `dim` set `dims` = '%s' where `appname`='%s' and `kpiname`='%s'" % (jdims, appname, kpiname)
        print sql
        cur.execute(sql)
    except mdb.Error, e:
        print "Error %d : %s" % (e.args[0], e.args[1])
def sets(parms):
    appname  = parms[0]
    kpiname  = parms[1]
    con = mdb.connect("10.75.26.127", "root", "123456", "monitor")
    cur = con.cursor()
    sql = "select `dims` from `dim` where `appname`='%s' and `kpiname`='%s'" % (appname, kpiname)
    cur.execute(sql)
    rows = cur.fetchall()
    dim=parms[2]
    value = float(parms[3])
    timestamp = int(time.time())
    sql = "insert into `kpis2`(`appname`,`kpiname`,`dim`,`time`,`value`) values ('%s','%s','%s','%d','%f')" % (appname, kpiname, dim, timestamp, value)
    cur.execute(sql)
def gets(parms):
    appname = parms[0]
    kpiname = parms[1]
    dim = parms[2]
    con = mdb.connect("10.75.26.127", "root", "123456", "monitor")
    cur = con.cursor()

    sql = "select `time`,value from `kpis2` where `appname`='%s' and `kpiname`='%s' and `dim`='%s'" % (appname, kpiname, dim)
    cur.execute(sql)
    rows = cur.fetchall()
    print "#time\t#value"
    for row in rows:
        print "%s\t%f" % (time.strftime('%Y%m%d %H:%M:%S',time.localtime(row[0])),row[1])

def dels(parms):
    appname = parms[0]
    kpiname = parms[1]
    dim = parms[2]
    con = mdb.connect("10.75.26.127", "root", "123456", "monitor")
    cur = con.cursor()
    sql = "delete from `kpis2` where `appname`='%s' and `kpiname`='%s' and `dim`='%s'" % (appname, kpiname, dim)
    cur.execute(sql)
    sql = "select `dims` from `dim` where `appname`='%s' and `kpiname`='%s'" % (appname, kpiname)
    cur.execute(sql)
    rows = cur.fetchall()
    ldims = set(json.loads(rows[0][0]))
    ldims.remove(dim)
    sql = "update `dim` set `dims` = '%s' where `appname`='%s' and `kpiname`='%s'" % (json.dumps(list(ldims)), appname, kpiname)
    cur.execute(sql)
    
    
    
if __name__ == '__main__':
    if len(sys.argv) <= 1:
        usage()
    if sys.argv[1] == "show":
        show(sys.argv[2])
    elif sys.argv[1] == "new":
        new(sys.argv[2], int(sys.argv[3]), sys.argv[4])
    elif sys.argv[1] == "kpi":
        kpi(sys.argv[2], sys.argv[3])
    elif sys.argv[1] == "dim":
        dim(sys.argv[2], sys.argv[3], sys.argv[4])
    elif sys.argv[1] == "set":
        sets(sys.argv[2:])
    elif sys.argv[1] == "get":
        gets(sys.argv[2:])
    elif sys.argv[1] == "del":
        dels(sys.argv[2:])
