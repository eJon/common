#!/usr/bin/python
#-*- coding:utf-8 -*-
# Copyright (c) 2012, Baidu.com Inc.
#
# Author      : hemingzhe <512284622@qq.com>; xiaorixin <xiaorx@live.com>
# Date        : Dec 20, 2012
#
 
import socket, logging
import select, errno
import os
import sys
import traceback
import Queue
import threading
import time
import thread
import cgi
from cgi import parse_qs
import json
import imp
from sendfile import sendfile
from os.path import join, getsize
import md5
import gzip
from StringIO import StringIO
from BaseHTTPServer import BaseHTTPRequestHandler
import re
 
logger = logging.getLogger("network-server")
action_dic = {}
action_time = {}
static_file_dir = "static"
static_dir = "/%s/" % static_file_dir
cache_static_dir = "cache_%s" % static_file_dir
if not os.path.exists(cache_static_dir):
    os.makedirs(cache_static_dir)
filedic = {"HTM":None,"HTML":None,"CSS":None,"JS":None,"TXT":None}
 
def getTraceStackMsg():
    tb = sys.exc_info()[2]
    msg = ''
    for i in traceback.format_tb(tb):
        msg += i
    return msg
 
def md5sum(fobj):
    m = md5.new()
    while True:
        d = fobj.read(65536)
        if not d:
            break
        m.update(d)
    return m.hexdigest()
 
class QuickHTTPRequest():
    def __init__(self, data):
        headend = data.find("\r\n\r\n")
        rfile = ""
        if headend > 0:
            rfile = data[headend+4:]
            headlist = data[0:headend].split("\r\n")
        else:
            headlist = data.split("\r\n")
        self.rfile = StringIO(rfile)
        first_line = headlist.pop(0)
        self.command, self.path, self.http_version =  re.split('\s+', first_line)
        indexlist = self.path.split('?')
        indexlist = indexlist[0].split('/')
        while len(indexlist) != 0:
            self.index = indexlist.pop()
            if self.index == "":
                continue
            else:
                self.action,self.method = os.path.splitext(self.index)
                self.method = self.method.replace('.', '')
                break
        self.headers = {}
        for item in headlist:
            if item.strip() == "":
                continue
            segindex = item.find(":")
            if segindex < 0:
                continue
            key = item[0:segindex].strip()
            value = item[segindex+1:].strip()
            self.headers[key] = value
        c_low = self.command.lower()
        self.getdic = {}
        self.form = {}
        self.postdic = {}
        if c_low  == "get" and "?" in self.path:
            self.getdic = parse_qs(self.path.split("?").pop())
        elif c_low == "post" and self.headers.get('Content-Type',"").find("boundary") > 0:
            self.form = cgi.FieldStorage(fp=self.rfile,headers=None,
                    environ={'REQUEST_METHOD':self.command,'CONTENT_TYPE':self.headers['Content-Type'],})
            if self.form == None:
                self.form = {}
        elif c_low == "post":
            self.postdic = parse_qs(rfile)
 
def sendfilejob(request, data, epoll_fd, fd):
    #print request.path,"3:",time.time()
    try:
 
        base_filename = request.path[request.path.find(static_dir)+1:]
        cache_filename = "./cache_"+base_filename
        filename = "./"+base_filename
        if not os.path.exists(filename):
            raise Exception("file not found")
        name,ext = os.path.splitext(filename)
        ext = ext.replace('.', '')
        iszip = False
        etag = request.headers.get("If-Modified-Since", None)
        #filemd5 = md5sum(file(filename))
        filemd5 = str(os.path.getmtime(filename))
        if ext.upper() in filedic:
            if not os.path.exists(cache_filename) or (etag != None and etag != filemd5):
                d,f = os.path.split(cache_filename)
                try:
                    if not os.path.exists(d):
                        os.makedirs(d)
                    f_out = gzip.open(cache_filename, 'wb')
                    f_out.write(open(filename).read())
                    f_out.close()
                except Exception, e:
                    print str(e)
                    pass
            filename = cache_filename
            iszip = True
 
 
 
        sock = data["connections"]
        #sock.setblocking(1)
        if etag == filemd5:
            #sock.send("HTTP/1.1 304 Not Modified\r\nLast-Modified: %s\r\n\r\n" % filemd5)
            data["writedata"] = "HTTP/1.1 304 Not Modified\r\nLast-Modified: %s\r\n\r\n" % filemd5
        else:
            data["sendfile"] = True
            sock.setblocking(1)
            offset = 0
            filesize = os.path.getsize(filename)
            f = open(filename, "rb")
            if iszip:
                headstr = "HTTP/1.0 200 OK\r\nContent-Length: %s\r\nLast-Modified: %s\r\nContent-Encoding: gzip\r\n\r\n" % (filesize,filemd5)
            else:
                headstr = "HTTP/1.0 200 OK\r\nContent-Length: %s\r\nLast-Modified: %s\r\n\r\n" % (filesize,filemd5)
            sock.send(headstr)
            while True:
                sent = sendfile(sock.fileno(), f.fileno(), offset, 65536)
                if sent == 0:
                    break  # EOF
                offset += sent
            f.close()
            data["writedata"] = ""
            data["sendfile"] = False
        #print request.path,"4:",time.time()
    except Exception, e:
        data["sendfile"] = False
        #data["writedata"] = str(e)+getTraceStackMsg()
        data["writedata"] = "file not found"
        pass
    try:
        data["readdata"] = ""
        epoll_fd.modify(fd, select.EPOLLET | select.EPOLLOUT | select.EPOLLERR | select.EPOLLHUP)
    except Exception, e:
        print str(e)+getTraceStackMsg()
 
class Worker(object):
 
    def __init__(self):
        pass
 
    def process(self, data, epoll_fd, fd):
        res = ""
        add_head = ""
        try:
            request = QuickHTTPRequest(data["readdata"])
        except Exception, e:
            #return "http parser error:"+str(e)+getTraceStackMsg()
            res = "http format error"
        try:
            headers = {}
            headers["Content-Type"] = "text/html;charset=utf-8"
            if request.path == "/favicon.ico":
                request.path = "/"+static_file_dir+request.path
            if static_dir in request.path or "favicon.ico" in request.path:
                thread.start_new_thread(sendfilejob, (request,data,epoll_fd,fd))
                #sendfilejob(request,data,epoll_fd,fd)
                return None
            action = action_dic.get(request.action, None)
            if action == None:
                action = __import__(request.action)
                mtime = os.path.getmtime("./%s.py" % request.action)
                action_time[request.action] = mtime
                action_dic[request.action] = action
            else:
                load_time = action_time[request.action]
                mtime = os.path.getmtime("./%s.py" % request.action)
                if mtime>load_time:
                    action = reload(sys.modules[request.action])
                    action_time[request.action] = mtime
                    action_dic[request.action] = action
 
            method = getattr(action, request.method)
            res = method(request, headers)
            if headers.get("Connection","") != "close":
                data["keepalive"] = True
            res_len = len(res)
            headers["Content-Length"] = res_len
            for key in headers:
                add_head += "%s: %s\r\n" % (key, headers[key])
        except Exception, e:
            #res = str(e)+getTraceStackMsg()
            res = "page no found"
        try:
            data["writedata"] = "HTTP/1.1 200 OK\r\n%s\r\n%s" % (add_head, res)
            data["readdata"] = ""
            epoll_fd.modify(fd, select.EPOLLET | select.EPOLLOUT | select.EPOLLERR | select.EPOLLHUP)
        except Exception, e:
            print str(e)+getTraceStackMsg()
 
def InitLog():
    logger.setLevel(logging.DEBUG)
 
    fh = logging.FileHandler("network-server.log")
    fh.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.ERROR)
 
    formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    ch.setFormatter(formatter)
    fh.setFormatter(formatter)
 
    logger.addHandler(fh)
    logger.addHandler(ch)
 
class MyThread(threading.Thread):
    ind = 0
    def __init__(self, threadCondition, shareObject, **kwargs):
        threading.Thread.__init__(self, kwargs=kwargs)
        self.threadCondition = threadCondition
        self.shareObject = shareObject
        self.setDaemon(True)
        self.worker = Worker()
 
    def processer(self, args, kwargs):
        try:
            param = args[0]
            epoll_fd = args[1]
            fd = args[2]
            self.worker.process(param, epoll_fd, fd)
        except:
            print  "job error:" + getTraceStackMsg()
 
    def run(self):
        while True:
            try:
                args, kwargs = self.shareObject.get()
                self.processer(args, kwargs)
            except Queue.Empty:
                continue
            except :
                print "thread error:" + getTraceStackMsg()
 
class ThreadPool:
    def __init__( self, num_of_threads=10):
        self.threadCondition=threading.Condition()
        self.shareObject=Queue.Queue()
        self.threads = []
        self.__createThreadPool( num_of_threads )
 
    def __createThreadPool( self, num_of_threads ):
        for i in range( num_of_threads ):
            thread = MyThread( self.threadCondition, self.shareObject)
            self.threads.append(thread)
 
    def start(self):
        for thread in self.threads:
            thread.start()
 
    def add_job( self, *args, **kwargs ):
        self.shareObject.put( (args,kwargs) )
 
def run_main(listen_fd):
    try:
        epoll_fd = select.epoll()
        epoll_fd.register(listen_fd.fileno(), select.EPOLLIN | select.EPOLLET | select.EPOLLERR | select.EPOLLHUP)
    except select.error, msg:
        logger.error(msg)
 
    tp = ThreadPool(16)
    tp.start()
 
    params = {}
    def clearfd(fd):
        epoll_fd.unregister(fd)
        params[fd]["connections"].close()
        del params[fd]
 
    last_min_time = -1
    while True:
        epoll_list = epoll_fd.poll()
 
        for fd, events in epoll_list:
            cur_time = time.time()
            if fd == listen_fd.fileno():
 
                while True:
                    try:
                        conn, addr = listen_fd.accept()
                        #print "accept",time.time(),conn.fileno()
                        conn.setblocking(0)
                        epoll_fd.register(conn.fileno(), select.EPOLLIN | select.EPOLLET | select.EPOLLERR | select.EPOLLHUP)
                        conn.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                        #conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, True)
                        params[conn.fileno()] = {"addr":addr,"writelen":0, "connections":conn, "time":cur_time}
                    except socket.error, msg:
                        break
            elif select.EPOLLIN & events:
                #print "read",time.time()
                param = params[fd]
                param["time"] = cur_time
                datas = param.get("readdata","")
                cur_sock = params[fd]["connections"]
                while True:
                    try:
                        data = cur_sock.recv(102400)
                        if not data:
                            clearfd(fd)
                            break
                        else:
                            datas += data
                    except socket.error, msg:
                        if msg.errno == errno.EAGAIN:
                            #logger.debug("%s receive %s" % (fd, datas))
                            param["readdata"] = datas
                            len_s = -1
                            len_e = -1
                            contentlen = -1
                            headlen = -1
                            len_s = datas.find("Content-Length:")
                            if len_s > 0:
                                len_e = datas.find("\r\n", len_s)
                            if len_s > 0 and len_e > 0 and len_e > len_s+15:
                                len_str = datas[len_s+15:len_e].strip()
                                if len_str.isdigit():
                                    contentlen = int(datas[len_s+15:len_e].strip())
                            headend = datas.find("\r\n\r\n")
                            if headend > 0:
                                headlen = headend + 4
                            data_len = len(datas)
                            if (contentlen > 0 and headlen > 0 and (contentlen + headlen) == data_len) or \
                                    (contentlen == -1 and headlen == data_len):
                                tp.add_job(param,epoll_fd,fd)
                                #print "1:",time.time()
                                #thread.start_new_thread(process, (param,epoll_fd,fd))
                            break
                        else:
                            clearfd(fd)
                            logger.error(msg)
                            break
            elif select.EPOLLHUP & events or select.EPOLLERR & events:
                clearfd(fd)
                logger.error("sock: %s error" % fd)
            elif select.EPOLLOUT & events:
                #print "write",time.time()
                param = params[fd]
                param["time"] = cur_time
                sendLen = param.get("writelen",0)
                writedata = param.get("writedata", "")
                total_write_len = len(writedata)
                cur_sock = params[fd]["connections"]
                if writedata == "":
                    clearfd(fd)
                    continue
                while True:
                    try:
                        sendLen += cur_sock.send(writedata[sendLen:])
                        if sendLen == total_write_len:
                            if param.get("keepalive", True):
                                param["readdata"] = ""
                                param["writedata"] = ""
                                param["writelen"] = 0
                                epoll_fd.modify(fd, select.EPOLLET | select.EPOLLIN | select.EPOLLERR | select.EPOLLHUP)
                            else:
                                clearfd(fd)
                            break
                    except socket.error, msg:
                        if msg.errno == errno.EAGAIN:
                            param["writelen"] = sendLen
                            break
            else:
                continue
            #check time out
            if cur_time - last_min_time > 10:
                last_min_time = cur_time
                objs = params.items()
                for (key_fd,value) in objs:
                    fd_time = value.get("time", 0)
                    del_time = cur_time - fd_time
                    if del_time > 10:
                        sendfile = value.get("sendfile", False)
                        if sendfile != True:
                            clearfd(key_fd)
                    elif fd_time < last_min_time:
                        last_min_time = fd_time
 
if __name__ == "__main__":
    InitLog()
    port = int(sys.argv[1])
    try:
        listen_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
    except socket.error, msg:
        logger.error("create socket failed")
    try:
        listen_fd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    except socket.error, msg:
        logger.error("setsocketopt SO_REUSEADDR failed")
    try:
        listen_fd.bind(('', port))
    except socket.error, msg:
        logger.error("bind failed")
    try:
        listen_fd.listen(1024)
        listen_fd.setblocking(0)
    except socket.error, msg:
        logger.error(msg)
 
    child_num = 8
    c = 0
    while c < child_num:
        c = c + 1
        newpid = os.fork()
        if newpid == 0:
            run_main(listen_fd)
    run_main(listen_fd)
