// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/path/filepath.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "path/filepath.h"

NSPIO_DECLARATION_START

enum {
    DEEPWALK_ENTRY = 0,
    DEEPWALK_FILE = 1,
    DEEPWALK_DIR = 2,
};


string Abs(const string &path) {
    string abs, cwd;
    char *__cwd = NULL;
    const char *ph = NULL;


    if (path.empty()) {
	__cwd = get_current_dir_name();
	cwd.assign(__cwd);
	free(__cwd);
	return cwd;
    }
    ph = path.data();
    if (ph[0] == '/') {
	return path;
    }
    __cwd = get_current_dir_name();
    cwd.assign(__cwd);
    free(__cwd);
    return Join(cwd, path);
}
    
string Base(const string &path) {
    string base;
    const char *ph = NULL;
    int plen = 0, i = 0;
    
    if (path.empty())
	return base;
    ph = path.data();
    i = plen = path.length();
    while (i) {
	if (ph[i - 1] == '/') {
	    base.assign((ph + i));
	    break;
	}
	i--;
    }
    if (base.empty())
	base =  path;
    return base;
}
    
string Dir(const string &path) {
    string dir;
    const char *ph = NULL;
    int plen = 0, i = 0;
    
    if (path.empty())
	return dir;
    ph = path.data();
    i = plen = path.length();
    while (i) {
	if (ph[i - 1] == '/') {
	    dir.assign(path.data(), i);
	    break;
	}
	i--;
    }
    return dir;
}    

bool IsAbs(const string &path) {
    const char *ph = NULL;
    if (path.empty())
	return false;
    ph = path.c_str();
    if (ph[0] != '/')
	return false;
    return true;
}
    
string Join(const string &path, const string &path2) {
    return path + "/" + path2;
}

bool HasPrefix(const string &path, const string &prefix) {
    return path.size() >= prefix.size() &&
	memcmp(path.data(), prefix.data(), prefix.size()) == 0;
}

bool HasSuffix(const string &path, const string &suffix) {
    return path.size() >= suffix.size() &&
	memcmp(path.data() + path.size() - suffix.size(), suffix.data(), suffix.size()) == 0;
}



FilePath::FilePath() :
    dtyp(DEEPWALK_ENTRY), cur_deep(0), max_deep(0)
{

}

FilePath::~FilePath() {

}


int FilePath::__walk(const string &pathname, walkFn __walkfn, void *data) {

    string subpath;
    DIR *dp = NULL;
    struct stat finfo = {};
    struct dirent *dir = NULL;

    if ((dp = opendir(pathname.c_str())) == NULL)
	return -1;
    while ((dir = readdir(dp)) != NULL) {
	if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
	    continue;
	subpath.clear();
	subpath = pathname + "/";
	subpath.append(dir->d_name);
	if (stat(subpath.c_str(), &finfo) < 0)
	    continue;
	switch (dtyp) {
	case DEEPWALK_ENTRY: __dwalk(subpath, __walkfn, data);
	    break;
	case DEEPWALK_FILE: __file_dwalk(subpath, __walkfn, data);
	    break;
	case DEEPWALK_DIR: __dir_dwalk(subpath, __walkfn, data);
	    break;
	}

    }
    closedir(dp);
    return 0;
}



    
int FilePath::__dwalk(const string &pathname, walkFn __walkfn, void *data) {
    struct stat finfo = {};
    
    if (stat(pathname.c_str(), &finfo) < 0)
	return -1;
    __walkfn(pathname, data);
    if (S_ISDIR(finfo.st_mode)) {
	cur_deep++;
	if (max_deep == 0 || (max_deep > 0 && cur_deep < max_deep))
	    __walk(pathname, __walkfn, data);
	cur_deep--;
    }
    return 0;
}

    
int FilePath::__file_dwalk(const string &pathname, walkFn __walkfn, void *data) {
    struct stat finfo = {};
    
    if (stat(pathname.c_str(), &finfo) < 0)
	return -1;
    if (S_ISREG(finfo.st_mode)) {
	__walkfn(pathname, data);
    } else if (S_ISDIR(finfo.st_mode)) {
	cur_deep++;
	if (max_deep == 0 || (max_deep > 0 && cur_deep < max_deep))
	    __walk(pathname, __walkfn, data);
	cur_deep--;	
    }

    return 0;
}
    
int FilePath::__dir_dwalk(const string &pathname, walkFn __walkfn, void *data) {
    struct stat finfo = {};
    
    if (stat(pathname.c_str(), &finfo) < 0)
	return -1;
    if (S_ISDIR(finfo.st_mode))
	__walkfn(pathname, data);
    if (S_ISDIR(finfo.st_mode)) {
	cur_deep++;
	if (max_deep == 0 || (max_deep > 0 && cur_deep < max_deep))
	    __walk(pathname, __walkfn, data);
	cur_deep--;
    }

    return 0;
}

int FilePath::Setup(const string &path) {
    root = path.empty() ? "." : path;
    cur_deep = 0;
    return 0;
}    
   
int FilePath::Walk(walkFn __walkfn, void *data) {
    return DeepWalk(__walkfn, 1, data);
}

int FilePath::DeepWalk(walkFn __walkfn, int deep, void *data) {
    dtyp = DEEPWALK_ENTRY;
    cur_deep = 0;
    max_deep = deep;
    return __walk(root, __walkfn, data);
}

int FilePath::WalkDir(walkFn __walkfn, void *data) {
    return DeepWalkDir(__walkfn, 1, data);
}

int FilePath::DeepWalkDir(walkFn __walkfn, int deep, void *data) {
    dtyp = DEEPWALK_DIR;
    cur_deep = 0;
    max_deep = deep;
    return __walk(root, __walkfn, data);
}
    
int FilePath::WalkFile(walkFn __walkfn, void *data) {
    return DeepWalkFile(__walkfn, 1, data);
}

int FilePath::DeepWalkFile(walkFn __walkfn, int deep, void *data) {
    dtyp = DEEPWALK_FILE;
    cur_deep = 0;
    max_deep = deep;
    return __walk(root, __walkfn, data);
}
    
    

}


