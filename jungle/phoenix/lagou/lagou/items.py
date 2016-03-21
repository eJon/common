# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# http://doc.scrapy.org/en/latest/topics/items.html

import scrapy


class LagouItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    url = scrapy.Field()
    job_type = scrapy.Field()
    title = scrapy.Field()
    department = scrapy.Field()
    require = scrapy.Field()
    duty = scrapy.Field()
    descript_more = scrapy.Field()
    salary = scrapy.Field()
    work_age = scrapy.Field()
    location = scrapy.Field()
    head_count = scrapy.Field()
    contact = scrapy.Field()
    pub_tm = scrapy.Field()
    expire_tm = scrapy.Field()
    update_tm = scrapy.Field()
    #    pass
