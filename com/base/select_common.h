#ifndef _SELECT_COMMON_H_
#define _SELECT_COMMON_H_

#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <list>
#include <vector>
using namespace std;


int parse_filter(const char *in_buffer, map<string, string> &filters, const char *delims);

int parse_filter(string &raw_filter, list<string> &filters, const char *delims);
int parse_filter(string &raw_filter, vector<string> &filters, const char *delims);

int get_last_files_mod_time(list<string> &files, time_t &last_mod_time);
int get_last_files_mod_time(string &file_name, time_t &last_mod_time);

#endif
