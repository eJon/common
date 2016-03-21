#!/usr/bin/env python
#_*_ encoding: utf-8 _*_

'''
Description:

'''

__author__ = 'Liu Wenyong'
__version__ = '0.1'
__date__ = '2012-03-27 20:10'

#import BeautifulSoup as bs
import platform
import time
import bs4 as bs
import chardet

class HtmlParser(object):
    """html parse class
    """

    def __init__(self, html,encode=""):
        if encode=="":
            detector = chardet.detect(html)
            encode = detector['encoding']
            if encode.lower()=="gb2312":
                encode = "gbk"
            print detector
        #print "use encoding="+encode+" to parse file"
        
        self.html_doc = html.decode(encode)
        self.root = bs.BeautifulSoup(self.html_doc)
        self.elements = []

        if not self.root: raise Exception("html is empty")

    def parse(self,xpathHasIndex = False):
        if xpathHasIndex:
            self.process_node_Index(self.root,"")
        else:
            self.process_node(self.root,"")
        
        return self.elements
    def process_node_Index(self,node,xpath):
        id = self.get_id(node)
        css = node.attrs
        js = ""
        url = self.get_url(node)
        txt = self.get_text(node)
        index = {}
        for child in node.findAll(recursive=False):
            if child.name not in index.keys():
                index[child.name] = 0
            index[child.name] += 1
            self.process_node_Index(child, xpath+"/"+child.name+"["+str(index[child.name])+"]")
        ele = (id, url, txt, xpath, css, js)
        self.elements.append(ele)
    def process_node(self, node,xpath):
        """parse node into an element tuple"""
        id = self.get_id(node)
        css = node.attrs
        js = ""
        url = self.get_url(node)
        # process text
        txt = self.get_text(node)
        # process tag, when recursive is false, get tags only
        for child in node.findAll(recursive=False):
            self.process_node(child, xpath+"/"+child.name)

        ele = (id, url, txt, xpath, css, js)
        self.elements.append(ele)
    def isTagNameInFontClass(self,TagName):
        FontClass=['h1','h2','h3','h4','h5','h6','b','strong','i','em','dfn','u','ins','strike','s','del','kbd','tt','xmp','plaintext','listing','font']
        num = len(FontClass)
        for item in FontClass:
            if TagName.lower() == item:
                return True
        return False
    def get_text(self, node):
        """ get text of an element"""
        txt = ""
        
        for child in node.contents:
           if isinstance(child,bs.Tag) and self.isTagNameInFontClass(child.name):
                if child.string and child.string != "":
                    txt += child.string.strip().replace("\n","  ")
           elif isinstance(child, bs.NavigableString):
                txt += child.string.strip().replace("\n","  ")
        return txt
    def get_css(self, node):
        return node.attrs
    def get_url(self,node):
        
        if not isinstance(node,bs.Tag):
            return ""
        if node.name != "a":
            return ""
        if not node.attrs:
            return ""
        for key in node.attrs.keys():
            if str(key.encode('utf-8'))=="href":
                return node.attrs[key].encode('utf-8')
        return ""
    def get_xpath(self,node):
        """return xpath of a tag"""
        if not isinstance(node,bs.Tag):
            return ""
        str1=""
        while node and not isinstance(node, bs.BeautifulSoup):
            str1 = "/"+node.name+str1
            node=node.parent

        return str1
    def get_xpath_withIndex(self,node):
        """return xpath(with index) of a tag"""
        if not isinstance(node,bs.Tag):
            return ""

        if isinstance(node,bs.NavigableString):            
            print "NavigableString type has no attribute xpath"           
            return ""
        xpath=""
        while node and not isinstance(node, bs.BeautifulSoup):
            index=1            
            if node.previous_siblings:                
                for sibling in node.previous_siblings:                    
                    if isinstance(sibling, bs.Tag) and sibling.name == node.name:                                       index += 1        
            xpath = "/"+node.name+"["+str(index)+"]"+xpath
            node = node.parent            
        return xpath
    def get_id(self, node):
        """return id of an tag ,if node is not a tag then null will be returned"""

        if not isinstance(node,bs.Tag):
            return ""
        if not node.attrs:
            return ""
        for key in node.attrs.keys():
            if str(key.encode('utf-8'))=="id":
                return node.attrs[key].encode('utf-8')
        return ""
    def get_js(self, node):
        return ""


    def attrsToStr(self,node):
        if isinstance(node,bs.NavigableString):
            print "NavigableString has no attrs attribute"
        str1="{"
        if not node.attrs:
            return ""
        for k,v in node.attrs:
            str1 += k.encode('utf-8')+":"+v.encode('utf-8')+";"
        str1 += "}"
        return str1
    def saveElementsToFile(self,elements,filename):
        """save html elements  to file """
        lines = ""
        for ele in elements:
            line = "<ID: "+ele[0].encode('utf-8')+"; "
            line += "url: "+ele[1].encode('utf-8')+"; "
            line += "txt: "+ele[2].encode('utf-8')+"; "
            line += "Xpath: "+ele[3].encode('utf-8')+"; "
            str1="{"
            for (k,v) in ele[4].items():
                str1 +=k.encode('utf-8')+": "+v.encode('utf-8')+";"
            str1+="#}"
            line += "css: "+str1+">\n"
            lines+=line

        file_obj = open(filename,'wb')
        file_obj.write(lines)
        file_obj.close()

    def getElementsInTuple(self,elements):
        """save html elements  to file """
        lines = []
        for ele in elements:

            line = "<ID: "+ele[0]+"; "
            line += "url: "+ele[1].encode('utf-8')+"; "
            line += "txt: "+ele[2].encode('utf-8')+"; "
            line += "Xpath: "+ele[3].encode('utf-8')+"; "
            str1="{"
            #print type(ele[4])
            for (k,v) in ele[4].itmes():
                str1 +=k.encode('utf-8')+": "+v.encode('utf-8')+";"
            str1+="}"
            line += "css: "+str1+">"
            lines.append(line)

        return lines

    def parseTagName(self,tagName):
        ret={}
        ret["name"] = ""
        ret["index"] = -1
        ret["attrs"] = {}
        ret["error"] = ""
        pos = tagName.find("[")
        if pos == -1:
            ret["name"] = tagName
        else:
            if tagName[0:pos]=="":
                ret["name"]=None
            else:
                ret["name"] = tagName[0:pos]
            end = tagName.find("]")
            if end == -1:
                ret["error"] = "[ and ] is not match"
                return ret
            index_attrs = tagName[pos+1 : end]
            pos = index_attrs.find("@")
            if pos == -1:
                index = index_attrs
                attrs = ""
            else:
                index = index_attrs[0:pos]
                attrs = index_attrs[pos+1:]
                items = attrs.split(";")
                for item in items:
                    pos = item.find("=")
                    if pos != -1:
                        key=item[0:pos]
                        key = key.strip()
                        value = item[pos+1:]
                        value=value.strip()
                        value = value.replace("'","")
                        value = value.replace('"',"")
                        ret["attrs"][key] = value
            if index=="":
                ret["index"] = -1
            elif index.isdigit() == True:
                ret["index"] = int(index)
            else:
                ret["error"] = "number between [] must be digit"  

        return ret    
            
    def get_element_by_xpath(self,Xpath,XpathHasIndex=False):
        ret=[]
        tag = []
        tag.append(self.root)
        saveXpath = Xpath
        if Xpath == "":
            return ret
        if Xpath[0] != "/":
            Xpath = "//" +Xpath
        while Xpath != "":
            ret=[]
            if Xpath =="/" or Xpath=="//":
                if Xpath=="/":
                    flag=False
                elif Xpath=="//":
                    flag=True
                Xpath=""
                retTagName = {"name":None,"index":-1,"attrs":{},"error":""}
                for item in tag:
                    result = self.search(item,retTagName,flag)
                    ret += result
                tag=ret
                continue
            if Xpath[0]=="/" and len(Xpath)>=2 and Xpath[1]=="/":
                Xpath = Xpath[2:]
                pos = Xpath.find("/")
                if pos == -1:
                    tagName = Xpath[0:]
                    Xpath = ""
                else:
                    tagName = Xpath[0:pos]
                    Xpath = Xpath[pos:]
                retTagName = self.parseTagName(tagName)
                if retTagName["error"] != "":
                    print retTagName["error"]
                    return []
                for item in tag:
                    result = self.search(item,retTagName,True)
                    ret += result
                                
            elif Xpath[0] == "/" and len(Xpath)>=2 and Xpath[1] != "/":
                #process /
                Xpath = Xpath[1:]
                pos = Xpath.find("/")
                if pos == -1:
                    tagName = Xpath[0:]
                    Xpath = ""
                else:
                    tagName = Xpath[0:pos]
                    Xpath = Xpath[pos:]
                retTagName = self.parseTagName(tagName)
                if retTagName["error"] != "":
                    print retTagName["error"]
                    return []
                for item in tag:
                    result = self.search(item,retTagName,False)
                    ret += result
            else:
                print "error: xpath is not well format "
            tag = ret
        
        elements = self.transferTagToTuple(ret,XpathHasIndex)
        return elements
    def transferTagToTuple(self,tags,xpathHasIndex=False):
        ret=[]
        if xpathHasIndex:
            for tag in tags:
                id=self.get_id(tag)
                url=self.get_url(tag)
                txt=self.get_text(tag)
                xpath=self.get_xpath_withIndex(tag)
                css=tag.attrs
                js=''
                ele=(id, url, txt, xpath, css, js)
                ret.append(ele)
        else:
            for tag in tags:
                id=self.get_id(tag)
                url=self.get_url(tag)
                txt=self.get_text(tag)
                xpath=self.get_xpath(tag)
                css=tag.attrs
                js=''
                ele=(id, url, txt, xpath, css, js)
                ret.append(ele)
        return ret
    def search(self,node,searchCondition,flag=True):

        if not node:
            return []
        ret = []

        tagName = searchCondition["name"]
        index = searchCondition["index"]
        attrs = searchCondition['attrs']
        
        if index > 0 :
            ret = node.findAll(name=tagName,attrs=attrs,recursive=flag,limit=index)
            if len(ret)==index:
                ret1=[]
                ret1.append(ret[index-1])
                return ret1
            else:
                return []
        else:
            ret = node.findAll(tagName,attrs,recursive=flag)
            
        return ret
if __name__ == "__main__":

    html_doc = file("./data/hao123_1.html","rb").read()
    
    x = HtmlParser(html_doc)
    elements = x.parse(1)
    fileName="./data/hao123_1"+".txt" 
    x.saveElementsToFile(elements,fileName)
    ele = x.get_element_by_xpath("//div[2]")
    x.saveElementsToFile(ele,"xpath.txt")

