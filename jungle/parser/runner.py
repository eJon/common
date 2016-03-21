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
import os 
import time 
from htmlparser import HtmlParser
import getopt
import page_pb2
import pycurl
import cStringIO
 

def usage():
    print "usage:"
    print "\tparse -p sourceFile resultFile"
    print "\tparse -g sourceFile xpath resultFile"
    print "\tsourceFile must be format of html  and resultFile will be format of txt"
    print ""


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

def load(html,encode, xpaths):
    parser = HtmlParser(html,encode)
    parser.parse()
    for key in xpaths:
        xpath=xpaths.get(key)
        elements = parser.get_element_by_xpath(xpath,encode)
        value = elements[0][2].encode('utf-8')


def main():
    xpaths={'a':'/html/body/div[@id="hrs_mainOuter"]/div[@id="hrs_main"]/div[@id="hrs_mainContent"]/form[@id="dispatchRegistForm"]/div[@id="hrs_searchOuter"]/div[@id="hrs_jobDetail"]/dl[3]/div',
            'b':'/html/body/div[@id="hrs_mainOuter"]/div[@id="hrs_main"]/div[@id="hrs_mainContent"]/form[@id="dispatchRegistForm"]/div[@id="hrs_searchOuter"]/div[@id="hrs_jobDetail"]/dl[2]/div'}

    url='http://talent.baidu.com/baidu/web/templet1000/index/corpwebPosition1000baidu!getOnePosition?postIdEnc=D67BB2CDF8486F8C&brandCode=1&recruitType=2&lanType=1&operational=6637AA56FA08745E71A74EA6AC68D5FFF28F462DA4C19FB3FABC8882DE74DA1C0FDB6AEAC9F8C487108CCBE39D45983B54F375AA1CAE83E6A21F36A7DBB429FDA1AA45697C458F4ECFB8A8436BC90963071C77628434937929275A0141E9DC9B1C8FE41D11E20AFFA9511543837DA597ADA993F4A79495C679D35888897C39FD21D98BCC2FE67575CAED499E9C86325CC3D44DA1C73F4DA945C7FA90CE9460F2'
    html=get_page(url)
    load(html,"utf-8",xpaths)
    return

if __name__ == "__main__":
    reload( sys )
    sys.setdefaultencoding('utf-8')
    main()

