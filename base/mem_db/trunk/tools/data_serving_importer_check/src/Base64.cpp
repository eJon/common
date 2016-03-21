#include "Base64.h"

namespace helptool {

string Base64::encode (const string &src, int line_len) {
  string str;

  if (line_len > 0 && 0 != (line_len % 4)) {
    line_len = 0;
  }

  const char *src_buff = src.data ();
  const int src_len = src.size ();
  unsigned char tmp[] = {0, 0, 0};
  int len = 0;

  for (int i = 0; i < (src_len / 3); i++) {
    tmp[0] = *src_buff++;
    tmp[1] = *src_buff++;
    tmp[2] = *src_buff++;
    str += m_encode_table[ (tmp[0] & 0xFC) >> 2];
    str += m_encode_table[ ((tmp[0] << 4) | (tmp[1] >> 4)) & 0x3F];
    str += m_encode_table[ ((tmp[1] << 2) | (tmp[2] >> 6)) & 0x3F];
    str += m_encode_table[tmp[2] & 0x3F];
    len += 4;

    if (line_len > 0 && line_len == len) {
      str += "\r\n";
      len = 0;
    }
  }

  //对剩余数据进行编码
  switch (src_len % 3) {
    case 1:
      tmp[0] = *src_buff++;
      str += m_encode_table[ (tmp[0] & 0xFC) >> 2];
      str += m_encode_table[ (tmp[0] & 0x03) << 4];
      str += "==";
      break;

    case 2:
      tmp[0] = *src_buff++;
      tmp[1] = *src_buff++;
      str += m_encode_table[ (tmp[0] & 0xFC) >> 2];
      str += m_encode_table[ ((tmp[0] << 4) | (tmp[1] >> 4)) & 0x3F];
      str += m_encode_table[ ((tmp[1] & 0x0F) << 2)];
      str += "=";
      break;

    default:
      break;
  }

  return str;
}

string Base64::decode (const string &src) {
  const char *src_buff = src.data ();
  const int src_len = src.size ();

  string str;

  for (int k = 0, i = 0, value = 0; i < src_len; i++) {
    unsigned char u_char = src_buff[i];

    if ('\r' != u_char && '\n' != u_char) {
      switch (k) {
        case 0:
          value = m_decode_table[u_char] << 18;
          break;

        case 1:
          value += m_decode_table[u_char] << 12;
          str += (value & 0x00FF0000) >> 16;
          break;

        case 2:
          if ('=' != u_char) {
            value += m_decode_table[u_char] << 6;
            str += (value & 0x0000FF00) >> 8;
          }

          break;

        case 3:
          if ('=' != u_char) {
            value += m_decode_table[u_char];
            str += value & 0x000000FF;
          }

          break;
      }

      (++k) %= 4;
    }
  }

  return str;
}

bool Base64::isValid (const string &src) {
  const char *src_buff = src.data ();
  const int src_len = src.size ();

  unsigned char tmp[] = {0, 0, 0}; // 记录倒数三个字符
  int tmp_i = 3;    // tmp数组游标
  int real_len = 0; // 有效编码长度
  bool end_flag = true;   // 末尾标记
  int end_equal_cnt = 0;  // 末尾=个数

  for (int k = src_len - 1; k >= 0; --k) {
    unsigned char value = src_buff[k];

    switch (value) {
      case '\r':
      case '\n':
        break;

      case '=' :
        if (!end_flag || end_equal_cnt >= 2) {
          return false;
        } else {
          ++end_equal_cnt;
          ++real_len;

          if (tmp_i > 0) {
            tmp[--tmp_i] = value;
          }
        }

        break;

      default:
        if (m_decode_table[value] < 0) {
          return false;
        } else {
          end_flag = false;
          ++real_len;

          if (tmp_i > 0) {
            tmp[--tmp_i] = value;
          }
        }

        break;
    }
  }

  if ((real_len % 4) != 0) {
    return false;
  }

  switch (end_equal_cnt) {
    case 0:
      return true;
      break;

    case 1:
      if (('=' == tmp[2]) && ('=' != tmp[1])
          && (0 == (m_decode_table[tmp[1]] & 0xC3))) {
        return true;
      } else {
        return false;
      }

      break;

    case 2:
      if (('=' == tmp[2]) && ('=' == tmp[1])
          && (0 == (m_decode_table[tmp[0]] & 0xCF))) {
        return true;
      } else {
        return false;
      }

      break;
  }

  return false;
}

const char Base64::m_encode_table[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char Base64::m_decode_table[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  62, // '+'
  -1, -1, -1,
  63, // '/'
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
  -1, -1, -1, -1, -1, -1, -1,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
  -1, -1, -1, -1, -1, -1,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
  39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
  -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
}
