import re
import json


from scrapy.selector import Selector
try:
    from scrapy.spider import Spider
except:
    from scrapy.spider import BaseSpider as Spider
from scrapy.utils.response import get_base_url
from scrapy.utils.url import urljoin_rfc
from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor as sle


from itzhaopin.items import *
from itzhaopin.misc.log import *


class TencentSpider(CrawlSpider):
    name = "tencent"
    allowed_domains = ["tencent.com"]
    start_urls = [
        "http://hr.tencent.com/position.php"
    ]
    rules = [
#        Rule(sle(allow=("/position.php\?&start=\d{,4}#a")), follow=True, callback='parse_item'),
 #       Rule(sle(allow=("/position_detail\.php.+")), follow=True, callback='parse_detail_item')
        Rule(sle(allow=("/position.php\?&start=10#a")), follow=True, callback='parse_item'),
        Rule(sle(allow=("/position_detail\.php.+")), follow=True, callback='parse_detail_item')
    ]
    def parse_detail_item(self, response):
        items = []
        hxs = Selector(response)
        nodes= hxs.xpath('//*[@id="position_detail"]/div/table[1]/tr[3]/td/ul/li')

        require = []
        for s in nodes:
            require.append(s.xpath('text()').extract())

        nodes= hxs.xpath('//*[@id="position_detail"]/div/table[1]/tr[4]/td/ul/li')

        duty = []
        for s in nodes:
            duty.append(s.xpath('text()').extract())

        item=TencentItem()
        item['job_require'] = require
        item['job_duty'] = duty
        item['company'] = 'Tencent'
        item['url']=get_base_url(response)
        params = (item['url']).split('?')[-1]
        for p in params.split('&'):
            param = p.split('=')
            if(param[0] == 'id'):
                item['id'] = param[1]
                break
 #       print("detail----:" + item['url'])
        item['tp'] = 2
        items.append(item)

        return items

    

    def parse_item(self, response):
        items = []
        sel = Selector(response)
        base_url = get_base_url(response)
        sites_even = sel.css('table.tablelist tr.even')
        for site in sites_even:
            item = TencentItem()
            item['title'] = site.css('.l.square a').xpath('text()').extract()[0]
            relative_url = site.css('.l.square a').xpath('@href').extract()[0]
            item['url'] = urljoin_rfc(base_url, relative_url)
#            print("list----:" + item['url'])
            params = (item['url']).split('?')[-1]
            for p in params.split('&'):
                param = p.split('=')
                if(param[0] == 'id'):
                    item['id'] = param[1]
                    break
            item['catalog'] = site.css('tr > td:nth-child(2)::text').extract()[0]
            item['location'] = site.css('tr > td:nth-child(4)::text').extract()[0]
            item['head_count'] = site.css('tr > td:nth-child(3)::text').extract()[0]
            item['pub_time'] = site.css('tr > td:nth-child(5)::text').extract()[0]
            item['tp'] = 1
            items.append(item)
            #print repr(item).decode("unicode-escape") + '\n'

        sites_odd = sel.css('table.tablelist tr.odd')
        for site in sites_odd:
            item = TencentItem()
            item['title'] = site.css('.l.square a').xpath('text()').extract()[0]
            relative_url = site.css('.l.square a').xpath('@href').extract()[0]
            item['url'] = urljoin_rfc(base_url, relative_url)
            params = (item['url']).split('?')[-1]
            for p in params.split('&'):
                param = p.split('=')
                if(param[0] == 'id'):
                    item['id'] = param[1]
                    break
            item['catalog'] = site.css('tr > td:nth-child(2)::text').extract()[0]
            item['location'] = site.css('tr > td:nth-child(4)::text').extract()[0]
            item['head_count'] = site.css('tr > td:nth-child(3)::text').extract()[0]
            item['pub_time'] = site.css('tr > td:nth-child(5)::text').extract()[0]
            item['tp'] = 1
            items.append(item)
            #print repr(item).decode("unicode-escape") + '\n'

        info('parsed ' + str(response))
        return items


    def _process_request(self, request):
        info('process ' + str(request))
        return request

