#!/usr/bin/env python
#_*_ encoding: utf-8 _*_

'''
Description:
    common function def here
'''

__author__ = 'andrewPD'
__version__ = '0.1'
__date__ = '2014-12-19 17:29'

import sys
import os 
import time 
import pycurl
import cStringIO
 
class common(object):

    def __init__(self):
       return

    #### fetch html from url ####
    def get_page(self,url): 
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
            return ""
        html=buf.getvalue()
        buf.close()
        c.close()
        return html
