from scrapy.spider import BaseSpider
from scrapy.selector import HtmlXPathSelector
from scrapy.contrib.spiders import CrawlSpider,Rule
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor


class LagouSpider(BaseSpider):
    name = "lagou"
    allowed_domains = ["ren58.com"]
    start_urls = [
        "http://www.ren58.com/index.php?c=page&a=index&id=1"
        ]
    rules=[
        Rule(SgmlLinkExtractor(allow=(r'http://movie.douban.com/top250\?start=\d+.*'))),
        Rule(SgmlLinkExtractor(allow=(r'http://movie.douban.com/subject/\d+')),callback="parse_item"),      
    ]

    def parse_item(self, response):
        sel=HtmlXPathSelector(response)
        item=LagouItem()
        item['name']=sel.xpath('//*[@id="content"]/h1/span[1]/text()').extract()
#        item['year']=sel.xpath('//*[@id="content"]/h1/span[2]/text()').re(r'\((\d+)\)')
 #       item['score']=sel.xpath('//*[@id="interest_sectl"]/div/p[1]/strong/text()').extract()
  #      item['director']=sel.xpath('//*[@id="info"]/span[1]/a/text()').extract()
   #     item['classification']= sel.xpath('//span[@property="v:genre"]/text()').extract()
    #    item['actor']= sel.xpath('//*[@id="info"]/span[3]/a[1]/text()').extract()
        return item

    def parse(self, response):
#        filename = response.url.split("/")[-2]
#        open(filename, 'wb').write(response.body)

 #       hxs = HtmlXPathSelector(response)
        hxs = HtmlXPathSelector(text=response.body);
        xpath='//h1/a/@href'
        print hxs.xpath(xpath).extract()
        ex='''
        sites = hxs.path('/html/body/a')
        #sites = hxs.path('//ul/li')
        items = []
        for site in sites:
            item = LagouItem()
            item['title'] = site.path('a/text()').extract()
            item['link'] = site.path('a/@href').extract()
            item['desc'] = site.path('text()').extract()
            items.append(item)
            return items
        '''
