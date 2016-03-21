# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html

from scrapy import signals
import json
import codecs
from scrapy import log
from twisted.enterprise import adbapi
from scrapy.http import Request
from scrapy.exceptions import DropItem
from scrapy.contrib.pipeline.images import ImagesPipeline
import time
import MySQLdb
import MySQLdb.cursors
import socket
import select
import sys
import os
import errno
import string
from itzhaopin.items import *


class JsonWithEncodingTencentPipeline(object):
    def __init__(self):
        self.dbpool = adbapi.ConnectionPool('MySQLdb',
            db = 'qiaoke',
            user = 'root',
            passwd = 'pengdong',
            cursorclass = MySQLdb.cursors.DictCursor,
            charset = 'utf8',
            use_unicode = False,
                                            
        )
        self.merge_item = dict()

    def process_item(self, item, spider):
        query = self.dbpool.runInteraction(self._conditional_insert, item)
        return item

    def _merge2(self, item1,item2):
        if item1['tp'] == 2:
            return self._merge2(item2,item1)
        item=TencentItem()
        item['title'] = item1['title']
        item['catalog'] = item1['catalog']
        item['location'] = item1['location']
        item['head_count'] = item1['head_count']
        item['url'] = item1['url']
        item['pub_time'] = item1['pub_time']
        item['job_duty'] = item2['job_duty']
        item['job_require'] = item2['job_require']
#        print(item)
        return item
        
    
    def _merge(self, item):
        
        if item['id'] in self.merge_item.keys():#merge
            value=self.merge_item.get(item['id'])
            print("find one")
            if item['tp']==value['tp']: #list            
   #             print("same tp:" + str(item['tp']))
                return item
            else:
               return self._merge2(item,value)
        else:#insert
        #    print("insert url:" + item['url'])
            self.merge_item[item['id']] = item
            return item

    def _get_content(self, item):
        buff=''
        for li in item:
            buff += li[0]
        return buff

    def _conditional_insert(self, tx, item_orig):
#        for i in range(len(item['title'])):
        item = self._merge(item_orig)
        if item == item_orig:
            return
#            print("same item")
        else:
 #           print("new item")
            tx.execute('select * from qiaoke.job_detail where url= %s',(item['url'],))
            result = tx.fetchone()
            if result:
                #already exist,update
                print("update list")
                tx.execute('update job_detail set title = "%s",catalog="%s",location="%s",head_count="%s",pub_time="%s",job_duty = "%s",job_require ="%s" where url="%s"',\
                           (item['title'],item['catalog'],item['location'],item['head_count'], item['pub_time'],item['url'],\
                            self._get_content(item['job_duty']),self._get_content(item['job_require']),))
            else:#not exit insert
                print("insert list")
                tx.execute('insert into job_detail(url,title,catalog,location,head_count,pub_time,job_duty ,job_require,company) values(%s,%s,%s,%s,%s,%s,%s,%s,%s)',\
                           (item['url'],item['title'],item['catalog'],item['location'],item['head_count'], item['pub_time'],\
                            self._get_content(item['job_duty']),self._get_content(item['job_require']),item['company'],))



