// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/path/filepath.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _FILEPATH_H_
#define _FILEPATH_H_

#include <iostream>
#include "decr.h"


using namespace std;


NSPIO_DECLARATION_START

string Abs(const string &path);    
string Base(const string &path);
string Dir(const string &path);
string Ext(const string &path);
bool IsAbs(const string &path);
string Join(const string &path, const string &path2);
bool HasPrefix(const string &path, const string &prefix);
bool HasSuffix(const string &path, const string &suffix);

typedef void (*walkFn)(const string &path, void *data);


class FilePath {
 public:
    FilePath();
    ~FilePath();

    int Setup(const string &path);

    int WalkDir(walkFn dwalkFn, void *data = NULL);
    int DeepWalkDir(walkFn ddwalkFn, int deep = 0, void *data = NULL);
    int WalkFile(walkFn fwalkFn, void *data = NULL);
    int DeepWalkFile(walkFn dfwalkFn, int deep = 0, void *data = NULL);
    int Walk(walkFn __walkFn, void *data = NULL);
    int DeepWalk(walkFn __walkFn, int deep = 0, void *data = NULL);
    
 private:
    string root;
    int dtyp;
    int cur_deep, max_deep;

    int __walk(const string &path, walkFn __walkfn, void *data);
    int __dwalk(const string &path, walkFn __walkfn, void *data);
    int __file_walk(const string &path, walkFn __walkfn, void *data);
    int __file_dwalk(const string &path, walkFn __walkfn, void *data);
    int __dir_walk(const string &path, walkFn __walkfn, void *data);
    int __dir_dwalk(const string &path, walkFn __walkfn, void *data);
};












};



#endif   // _PATH_H_
