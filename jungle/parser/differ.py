#!/usr/bin/env python
#_*_ coding: utf-8 _*_

'''
Description: get different elements btw. two html

'''

__author__ = 'liu wen yong'
__version__ = '0.1'
__date__ = '2012-04-10 23:55'

import codecs
from htmlparser import HtmlParser
import cPickle as pickle
import time
import difflib
class differ(object):
    def __init__(self):
        pass


    @classmethod
    def diff_html(cls, file1_lines, file2_lines):
        '''get differences btw. two html documents
           input content format:  [(id, url, txt, xpath, css, js),...]
           output content format: [(type, txt_old, url_old, xpath_old, css_old, js_old,txt_new, url_new, xpath_new, css_new, js_new),...]

           visit each item in old elements, comparing each element with new item in new elements:
           if txt and url both equal, but xpath different, type = xpath_change
           if txt are equal but url are different, type = url_change
           if url equal but txt diff, type = txt_change
           if url and txt diff, type = delete
           else type = add
        '''
        d = difflib.Differ()
        #print file1_lines
        try:
            diffs = d.compare(file1_lines, file2_lines)
            lines = list(diffs)

        except Exception , e:
            print "compare error\n"
            print e
            return []
        num = len(lines)
        ########to save the diff middle value to file
        #i=0
        #str1=""
        #while i <num:
         #  str1 += str(i)+"("+lines[i]+")\n"
          # i=i+1
        #file_obj =file("abc.txt","wb")
        #file_obj.write(str1)
        #file_obj.close()
        ##########
        i=0
        ret=[]
        while i<num:
            if lines[i][0]==" ":
                i=i+1
            elif lines[i][0]=="-":
                if i+1<num and lines[i+1][0] == "+":
                    if i+2<num and lines[i+2][0] == "?":
                        #process "-+?" 
                        newLine = cls.process_TwoLines(lines[i],lines[i+1])
                        ret.append(newLine)
                        i=i+3
                    else:
                        #current line is deleted in second document;process it
                        newLine=cls.process_line(lines[i],"delete")
                        ret.append(newLine)
                        i=i+1
                elif i+1 < num and lines[i+1][0] == "?":
                    if i+2 < num and lines[i+2][0] == "+":
                        if i+3 < num and lines[i+3][0] == "?":
                            #process"-?+?"
                            newLine = cls.process_TwoLines(lines[i],lines[i+2])
                            ret.append(newLine)
                            i=i+4
                        else:
                            #process"-?+"
                            newLine = cls.process_TwoLines(lines[i],lines[i+2])
                            ret.append(newLine)
                            i=i+3
                    else :
                        #process error "-?" become to the last two lines
                        ret.append(lines[i])
                        i = i+2
                else:
                    #process lines[i] second lines delete
                    newLine=cls.process_line(lines[i],"delete")
                    ret.append(newLine)
                    i=i+1
            elif lines[i][0] == "+":
                    #process lines[i]  second doc add
                    newLine=cls.process_line(lines[i],"add")
                    ret.append(newLine)
                    i=i+1
            else :
                #process error lines[i][0]=?
                ret.append("---error---"+lines[i])
                i=i+1
        return ret
    @classmethod
    def saveDiffsToFile(cls,diffs,fileName):
        file_obj=file(fileName,"wb")
        file_obj.write("\n".join(diffs))
        file_obj.close()
        print "diff is save into file filename=  "+fileName
    @classmethod
    def process_TwoLines(cls,lines1,lines2):
        '''process two lines .type is change
            line1 is old and line2 is new
        '''
        ret=""
        lines1 = lines1[lines1.find("url:") : -1]+";"
        lines2 = lines2[lines2.find("url:") :]
        #get old content and new content
        url_old = lines1[lines1.find("url:") : lines1.find("txt:")-1]
        txt_old = lines1[lines1.find("txt:") : lines1.find("Xpath:")-1]
        Xpath_old = lines1[lines1.find("Xpath:") : lines1.find("css:")-1]
        
        url_new = lines2[lines2.find("url:") : lines2.find("txt:")-1]
        txt_new = lines2[lines2.find("txt:") : lines2.find("Xpath:")-1]
        Xpath_new = lines2[lines2.find("Xpath:") : lines2.find("css:")-1]
        
        if txt_old != txt_new:
            type="<type: txt_change; "
        elif url_old != url_new:
            type="<type: url_change; "
        elif Xpath_old != Xpath_new:
            type="<type: Xpath_change; "
        else:
            type="<type: css_change; "
        #replace old  and new
        lines1 = lines1.replace("url:","url_old:")
        lines1 = lines1.replace("txt:","txt_old:")
        lines1 = lines1.replace("Xpath:","Xpath_old:")
        lines1 = lines1.replace("css:","css_old:")
        
        lines2 = lines2.replace("url:","url_new:")
        lines2 = lines2.replace("txt:","txt_new:")
        lines2 = lines2.replace("Xpath:","Xpath_new:")
        lines2 = lines2.replace("css:","css_new:")
        
        lines1 = type + lines1
        return lines1+lines2
    @classmethod
    def process_line(cls,line,type):
        '''process a line. type should be "add" or "delete"
        '''
        line = line[line.find("url:") :]
        ret = ""
        if type=="add":
            ret = line.replace("txt:","txt_new:")
            ret = ret.replace("url:","url_new:")
            ret = ret.replace("Xpath:","Xpath_new:")
            ret = ret.replace("css:","css_new:")
            ret = "<type: add; url_old: ; txt_old: ; Xpath_old: ; css_old: {#} ; " + ret
        elif type=="delete":
            ret = line.replace("txt:","txt_old:")
            ret = ret.replace("url:","url_old:")
            ret = ret.replace("Xpath:","Xpath_old:")
            ret = ret.replace("css:","css_old:")
            ret = "<type: delete;"  + ret
            ret = ret.replace("}>","};url_new: ; txt_new: ; Xpath_new: ; css_new: {#}>")
        else:
            ret=line
        return ret
            
    @classmethod
    def diff_txt_from_file(cls, file_name1, file_name2):
        '''get different elements btw. two html documents
            file content contain elements like following:
            <ID: ; url: ; txt: ; Xpath: /html/body/div/div/div/div/div/div/div/div/div/a/span/img; css: {border: 0;alt: ;}>
        '''
        try:
            file1 = file(file_name1, 'rb').read()
            file1_lines=file1.split("#}>\n")
            ret1=[]
            for item in file1_lines:
                item=item+"#}>"
                ret1.append(item)
            file2 = file(file_name2, 'rb').read()
            file2_lines=file2.split("#}>\n")
            
            ret2=[]
            for item in file2_lines:
                item=item+"#}>"
                ret2.append(item)
            diffs = cls.diff_html(ret1,ret2)
            
            return diffs
        except Exception , e:
            print e


    @classmethod
    def diff_html_from_file(cls, fileName1, fileName2,encode):
        '''get different elements btw. two html files
        '''
        
        if fileName1=="" or fileName2=="":
            print "class differ : function :diff_html_from_file() fileName1 or fileName2 is null"
            return []
            
        html_str1 = file(fileName1,"rb").read()
        html_Parser1 = HtmlParser(html_str1,encode)
        elements1 = html_Parser1.parse() 
        html_Parser1.saveElementsToFile(elements1,"./tmp1.txt")
        
        html_str2 = file(fileName2,"rb").read()
        html_Parser2 = HtmlParser(html_str2,encode)
        elements2 = html_Parser2.parse()
        html_Parser2.saveElementsToFile(elements2,"./tmp2.txt")
        
        
            
        diffs = cls.diff_txt_from_file("tmp1.txt", "tmp2.txt")   
        return diffs
        
           

if __name__ == "__main__":
    try:
        
        diffs = differ.diff_txt_from_file("./data/hao123_2.txt","./data/hao123_1.txt")
       # diffs = differ.diff_html_from_file("./data/hao123.html","./data/hao123_1.html")
        differ.saveDiffsToFile(diffs,"dif_result.txt")
    except Exception , e:
        print e

