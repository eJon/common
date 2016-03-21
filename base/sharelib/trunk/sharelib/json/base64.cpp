
#include "sharelib/json/string_tools.h"
#include "sharelib/json/exception.h"
#include "sharelib/json/base64.h"

#include <string>
#include <iostream>

using namespace std;
SHARELIB_BS;

/*
 * in  xxxxxxxx xxxxxxxx xxxxxxxx
 * out 11111122 22223333 33444444
 */

void Base64Encoding(std::istream& is, std::ostream& os, char makeupChar, const char *alphabet)
{
    int out[4];
    int remain = 0;
    while (!is.eof())
    {
        int byte1 = is.get(); 
        if (byte1 < 0)
        {
            break;
        }
        int byte2 = is.get();
        int byte3;
        if (byte2 < 0)
        {
            byte2 = 0;
            byte3 = 0;
            remain = 1;
        }
        else
        {
            byte3 = is.get();
            if (byte3 < 0)
            {
                byte3 = 0;
                remain = 2;
            }
        }
        out[0] = static_cast<unsigned>(byte1) >> 2;
        out[1] = ((byte1 & 0x03) << 4) + (static_cast<unsigned>(byte2) >> 4);
        out[2] = ((byte2 & 0x0F) << 2) + (static_cast<unsigned>(byte3) >> 6);
        out[3] = byte3 & 0x3F;
                      
        if (remain == 1)
        {
            os.put(out[0] = alphabet[out[0]]);
            os.put(out[1] = alphabet[out[1]]);
            os.put(makeupChar);
            os.put(makeupChar);
        }
        else if (remain == 2)
        {
            os.put(out[0] = alphabet[out[0]]);
            os.put(out[1] = alphabet[out[1]]);
            os.put(out[2] = alphabet[out[2]]);
            os.put(makeupChar);
        }
        else
        {
            os.put(out[0] = alphabet[out[0]]);
            os.put(out[1] = alphabet[out[1]]);
            os.put(out[2] = alphabet[out[2]]);
            os.put(out[3] = alphabet[out[3]]);
        }
    }
}

//Base64Decoding
void Base64Decoding(std::istream& is, std::ostream& os, char plus, char slash)
{
    int out[3];
    int byte[4];
    int bTmp;
    int bTmpNext;
    const int numOfAlpha = 26;
    const int numOfDecimalNum = 10;
    const int numOfBase64 = 64;
    int index;
    
    while (is.peek() >= 0)
    {
        byte[0] = byte[1] = byte[2] = byte[3] = 0; 
        out[0] = out[1] = out[2] = 0;
        bTmp = 0;
        
        for (int i = 0; i < 4; i++)
        {
            bTmp = is.get();
                      
            if (bTmp == '=')
            {
                index = i;
                break;
            }
            else if (bTmp >= 'A' && bTmp <= 'Z')
            {
                byte[i] = bTmp - 'A';
            }
            else if (bTmp >= 'a' && bTmp <= 'z')
            {
                byte[i] = bTmp - 'a' + numOfAlpha;
            }
            else if (bTmp >= '0' && bTmp <= '9')
            {
                byte[i] = bTmp + numOfAlpha * 2 - '0';
            }
            else if (bTmp == plus)
            {
                byte[i] = numOfAlpha * 2 + numOfDecimalNum; 
            }
            else if (bTmp == slash)
            {
                byte[i] = numOfBase64 - 1;
            }
            else if (bTmp < 0)
            {
                SHARELIB_JSON_THROW(BadBase64Exception, "\n\nInvaild input!\nInput must be a multiple of four!!\n");
            }
            else
            {
                SHARELIB_JSON_THROW(BadBase64Exception, "\n\nInvaild input!\nPlease input the character in the string below:\nABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=\n");
            }
        } 
    
        out[0] = (byte[0] << 2) + (static_cast<unsigned>(byte[1]) >> 4) ;
        out[1] = ((byte[1] & 0x0F) << 4) + (static_cast<unsigned>(byte[2]) >> 2); 
        out[2] = ((byte[2] & 0x03) << 6) + byte[3];
                
        if (bTmp == '=')
        {
            if (index == 0 || index == 1)
            {
                SHARELIB_JSON_THROW(BadBase64Exception, "\n\nInvaild input!\nInput must be a multiple of four!!\n= must be the third or fourth place in the last four characters!\n");
            }
            else if (index == 2)
            {
                bTmpNext = is.get();
                if (bTmpNext == '=')
                {
                    if (is.peek() < 0)
                    {
                        os.put(out[0]);                   
                    }
                    else
                    {   
                        SHARELIB_JSON_THROW(BadBase64Exception, "\n\nInvaild input!\nPlease do not append any character after == !\n");
                    }
                }
                else
                {
                    SHARELIB_JSON_THROW(BadBase64Exception, "\n\nInvaild input!\nPlease do not append any character after = except = !\n");
                }
            }
            else
            {
                if (is.peek() < 0)
                {
                    os.put(out[0]);
                    os.put(out[1]);
                }
                else
                {
                    SHARELIB_JSON_THROW(BadBase64Exception, "\n\nInvaild input!\nInput must be a multiple of four!!\nPlease do not append any character after the first = !\n");
                }
            }
        }
        else
        {
            os.put(out[0]);
            os.put(out[1]);
            os.put(out[2]); 
        }           
    }
}

SHARELIB_ES;
