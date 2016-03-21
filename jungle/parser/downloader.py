#!/usr/bin/env python
#_*_ encoding: utf-8 _*_

'''
Description:

'''

__author__ = 'Chen Wei'
__version__ = '0.1'
__date__ = '2012-03-26 01:46'

import time
import htmlparser
import logging
from win32com.client import DispatchEx
class Downloader(object):
    def __init__(self,url):
        self.url = ""
        self.ie = DispatchEx('InternetExplorer.Application')
        self.ie.Visible=0
        self.url=url
        self.ie.Navigate(url)         
        currentTime=int(time.time())
        while self.ie.ReadyState != 4:
            if int(time.time()) > currentTime+10:
                logging.warning("time out after 10s")
                break
            time.sleep(1)            
        self.fileName=""
        self.html=""
    def __del__(self):
        self.ie.Quit()

    def get_html(self):
        self.html = self.ie.Document.getElementsByTagName('html')[0].outerHTML
        return self.html
    def write_file(self):

        fileName=self.url.replace("http://","")
        fileName=fileName.replace("https://","")

        if fileName.find("?")>0:
            fileName=fileName[0:fileName.find("?")]
        fileName=fileName.replace("/","_")+"-"+time.strftime("%H-%M-%S")
        self.fileName=fileName
    
        file_obj=open(fileName+".html","wb")
        file_obj.write(self.html.encode('utf-8'))
        file_obj.close()

    def saveHtmlToFile(self,fileName):
        file_obj=open(fileName,"wb")
        file_obj.write(self.html.encode('utf-8'))
        file_obj.close()

if __name__ == "__main__":
    url = "http://baike.baidu.com/view/1758.htm"
    #url = "http://db-testing-wiki00.db01.baidu.com:8080/test/index.php"
    downloader = Downloader(url)
    downloader.get_html()
    downloader.write_file()

