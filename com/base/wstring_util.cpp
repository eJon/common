#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <mscom/base/wstring_util.h>
using namespace std;
namespace ms {
namespace common {

int String::substring(const char *str, int len, string &rest) {
    if(len < 1 || 4 * len >= MAX_STR_BUFFER_LEN) {
        return 0;
    }

    const char *result;
    result = str;
    char str_buffer[MAX_STR_BUFFER_LEN];
    memset(str_buffer, 0, sizeof(char) * (MAX_STR_BUFFER_LEN));
    int i = 0;

    while(result != NULL && *result != '\0' && len > 0) {
        if((*result > 0x80 || *result < 0)) {  //chinese high
            for(int j = 0; j < 3; j++) {
                if(result != NULL && *result != '\0') {
                    str_buffer[i++] = *result;
                    result++;
                }
            }

            len--;
            continue;
        }

        str_buffer[i++] = *result;
        result++;
        len--;
    }

    for(int j = 0; j < 3; j++) {
        str_buffer[i++ ] = '.';
    }

    str_buffer[i++ ] = '\0';
    rest = str_buffer;
    return 1;

}
}
}

