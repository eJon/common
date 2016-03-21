# Define here the models for your scraped items
#
# See documentation in:
# http://doc.scrapy.org/en/latest/topics/items.html

from scrapy.item import Item, Field

class TencentItem(Item):
    title = Field()
    catalog = Field()
    location = Field()
    head_count = Field()
    pub_time = Field()
    job_duty = Field()
    job_require = Field()
    url = Field()
    tp = Field()
    id = Field()
    company = Field()

