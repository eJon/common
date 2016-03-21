import sys
import web
        
urls = (
    '/', 'hello',
    '/ad', 'ad'
)
app = web.application(urls, globals())
#### global def of DB####

DBNAME='test'
DBUSER='root'
DBPWD='pengdong'
DBNAME='test'
DBTABLE='ad'

def request_ad(id):
    db = web.database(dbn='mysql', user='root', pw='pengdong', db='test')
    w = "id=" + id
    result = db.select('job',where=w,what="keyword")
    if len(result) == 0:
        return "null"
    keywords= result[0]['keyword'].split(',')
    li=[]
    for keyword in keywords:
        w = "keyword like '%" + keyword + "%'"
        result = db.select('ad',where=w,what="id")
        if len(result) == 0:
            continue
        li.append(str(result[0]["id"]))

    return ','.join(li)

class ad:
    def GET(self):
        res = ""
        if 'id' in web.input():
            res = request_ad(web.input()['id'])
        return res

class hello:        
    def GET(self, name):
        if not name: 
            name = 'World'
        return 'Hello, ' + name + '!'

if __name__ == "__main__":
    app.run()
