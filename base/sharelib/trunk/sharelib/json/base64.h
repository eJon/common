#ifndef SHARELIB_JSON_BASE64_H
#define SHARELIB_JSON_BASE64_H

#include "sharelib/common.h"
#include "sharelib/json/exception.h"
#include <iostream>
SHARELIB_BS;

class BadBase64Exception : public ExceptionBase
{
public:
    SHARELIB_JSON_DEFINE_EXCEPTION(BadBase64Exception, ExceptionBase);
};

void Base64Encoding(std::istream&, std::ostream&, char makeupChar = '=',
                    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
    
void Base64Decoding(std::istream&, std::ostream&, char plus = '+', char slash = '/');

SHARELIB_ES;

#endif

