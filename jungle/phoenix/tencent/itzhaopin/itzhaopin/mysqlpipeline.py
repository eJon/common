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
#连接数据库
class MySQLStorePipeline(object):
    def __init__(self):
        self.dbpool = adbapi.ConnectionPool('MySQLdb',
            db = 'test',
            user = 'root',
            passwd = 'pengdong',
            cursorclass = MySQLdb.cursors.DictCursor,
            charset = 'utf8',
            use_unicode = False
        )
    #pipeline默认调用
    def process_item(self, item, spider):
        query = self.dbpool.runInteraction(self._conditional_insert, item)
        return item

    #将每行写入数据库中
    def _conditional_insert(self, tx, item):
        for i in range(len(item['title'])):
            tx.execute('select * from test.job_detail where url= %s',(item['url'][i],))
            result = tx.fetchone()
            if result:
                #already exist,update
                if(item['tp']==1):#list
                    tx.execute('update test.job_detail set title = %s,catalog=%s,location=%s,head_count=%d,pub_time=%s where url=%s',\
                               (item['title'][i],item['catalog'][i],item['location'][i],item['head_count'][i], item['pub_time'][i],item['url'][i]))
                else:#detail
                    tx.execute('update test.job_detail set job_duty = %s,job_require =%s where url=%s',\
                               (item['job_duty'][i],item['job_require'][i],item['url'][i]))
            else:#not exit insert
                if(item['tp']==1):#list
                    tx.execute('insert into test.job_detail(url,title,catalog,location,head_count,pub_time) values(%s,%s,%s,%s,%d,%s)',\
                               (item['url'][i],item['title'][i],item['catalog'][i],item['location'][i],item['head_count'][i], item['pub_time'][i]))
                else:#detail
                    tx.execute('insert into test.job_detail(url,job_duty ,job_require) values(%s,%s,%s) ',\
                               (item['url'][i],item['job_duty'][i],item['job_require'][i]))


