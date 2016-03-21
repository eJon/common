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
sys.path.append("..")
import os 
import time 
from htmlparser import HtmlParser
import getopt
import page_pb2
import pycurl
import cStringIO
import md5
import base64
 
def get_page(url): 
    buf = cStringIO.StringIO()
    c = pycurl.Curl()
    c.setopt(c.URL, url)
    c.setopt(c.CONNECTTIMEOUT, 5)
    c.setopt(c.TIMEOUT, 8)
    c.setopt(c.COOKIEFILE, '')
    c.setopt(c.FAILONERROR, True)
    c.setopt(c.HTTPHEADER, ['Accept: text/html', 'Accept-Charset: UTF-8'])
    c.setopt(c.WRITEFUNCTION, buf.write)
    try:
        c.perform()
    except pycurl.error, error:
        errno, errstr = error
        print 'An error occurred: ', errstr
    html=buf.getvalue()
    buf.close()
    c.close();
    return html

def set_pb(pb,k,v):
    return

def load(id,html,encode, xpaths):
    parser = HtmlParser(html,encode)
    parser.parse()
    jd = page_pb2.JobDescription()
    js ="{";
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

def useage():
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
        count=0
        for line in lines:
            h = md5.new()
            h.update('line')
            ec = h.digest()
            id= h.hexdigest()
            html=get_page(line.rstrip('\n'))
            load(id,html,"utf-8",xpaths)
            count=count+1
        fd.close()
        print "[OK] handle " + str(count) + " records"
    return

if __name__ == "__main__":
    reload( sys )
    sys.setdefaultencoding('utf-8')
    main()

