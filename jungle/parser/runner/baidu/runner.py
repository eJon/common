#!/usr/bin/env python
#_*_ encoding: utf-8 _*_

from __future__ import with_statement
import ConfigParser

'''
Description:
    parse html file to tuples save in txt file
'''

__author__ = 'andrewPD'
__version__ = '0.1'
__date__ = '2014-12-19 17:29'

import sys
sys.path.append("../..")
import os 
import time 
from htmlparser import HtmlParser
import getopt
import page_pb2
import pycurl
import cStringIO
import md5
import base64
from com import common
 
def set_pb(pb,k,v):
    return

def load(id,tm,url,html,encode, xpaths):
    parser = HtmlParser(html,encode)
    parser.parse()

    db_sql =  "insert into job_detail(url,src_desc,type,title,\
    keywords,department,job_require,job_duty,\
    job_welfare,label,company,company_desc,\
    logo,salary,work_experience,\
    edu, field,location,head_count,pub_time) values("

    jd = page_pb2.JobDescription()
    js ="{\"pub_tm\":\"" + tm + "\","
    js = js + "\"url\":\"" + url + "\","
    for key in xpaths:
#        print "[ON]handle " + key
        xpath=xpaths.get(key)
        elements = parser.get_element_by_xpath(xpath,encode)
        if (len(elements) == 0):
            print "[ERR] " + key
            continue
        value = elements[0][2].encode('utf-8')
        js += "\"" + key + "\":\"" + value + "\","
#        set_pb(jd,key,value)
    fp=open("./data/"+id+".dat",'w')
    fp.write(js.rstrip(',') + "}")
    fp.close()

def load_xpath(file_name):
    fd=open(file_name,"r+")
    lines=fd.readlines()
    xpaths={}
    for line in lines:
        kv = line.split(':')
        if(len(kv) == 2):
            xpaths[kv[0]] = kv[1].rstrip('\n')
        else:
            print "len err"
    fd.close()
#    print "[ON]len of xpath:" + str(len(xpaths))
    return xpaths

def usage():
    print "python runner.py baidu/baidu_xpaths.conf baidu/baidu_seeds.conf"

def main():
    argvs = sys.argv[1:]
    #load from file
    if(len(argvs) != 2):
        usage()
        return
    else:
        xpaths=load_xpath(argvs[0])
        fd = open(argvs[1],"r+")
        lines=fd.readlines()
        com = common()
        count=0
        for line in lines:
            h = md5.new()
            kv = line.split(' ')
            if(len(kv) != 2):
               continue
            tm = kv[0]
            url=kv[1]
            h.update(url)
            ec = h.digest()
            id= h.hexdigest()
            html=com.get_page(url.rstrip('\n'))
            load(id,tm,url,html,"utf-8",xpaths)
            count=count+1
        fd.close()
        print "[OK] handle " + str(count) + " records"
    return

if __name__ == "__main__":
    reload( sys )
    sys.setdefaultencoding('utf-8')
    main()

