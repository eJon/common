/////////////////////////////////////////////////////////////////////////////
//
//  Project                     ___| | | |  _ \| |
//                             / __| | | | |_) | |
//                            | (__| |_| |  _ <| |
//                             \___|\___/|_| \_\_____|
//
// Copyright (C) 2013 - 2020, Tieniu Li, <tieniu@staff.sina.com.cn>, et al.
//
// This software is licensed as described in the file COPYING, which
// you should have received as part of this distribution. The terms
// are also available at http://curl.haxx.se/docs/copyright.html.
//
// You may opt to use, copy, modify, merge, publish, distribute and/or sell
// copies of the Software, and permit persons to whom the Software is
// furnished to do so, under the terms of the COPYING file.
//
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express or implied.
//
// This software is modified based on ./examples/sendrecv.c of curl source
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _LIVE_CURL_H_
#define _LIVE_CURL_H_

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
using namespace std;

enum ErrorCode {
    SUCCESS = 0,
    EASY_CRUL_API_EXECUTION_ERROR = 1,
    CONNECT_ERROR = 2,
    SEND_DATA_ERROR = 3,
    RECV_DATA_ERROR = 4,
    SELECT_ERROR = 5,
    TIMEOUT_ERROR = 6,
    SEND_TIMEOUT_ERROR = 7,
    RECV_TIMEOUT_ERROR = 8,
    RETRY_REQUEST_AGAIN = 9,
    RECV_FAILED = 10,
    TIMEOUT = 11
};

// class LiveCurl for keep alive curl request for curl pulling
// features:
//   1. keep alive url request
//   2. timeout setting for sending
//   3. timeout setting for receiving
//   4. receivign action can finish in advance if the actual "response_end_flag" is set,
//      which will avoid the cost of the last unneccessary receiving timeout
//   5. get the port of the keepalive connection
//   6. multi-thread support, but MUST individual instance per thread
class LiveCurl {
  public:
    LiveCurl() : curl_(NULL), sockfd_(0), connect_timeout_ms_(0), transfer_timeout_ms_(0) {}
    ~LiveCurl() {}

    int Init(const std::string &host, const std::string &uri,
             int connect_timeout_ms, int transfer_timeout_ms) {
        host_ = host;
        uri_ = uri;
        connect_timeout_ms_ = connect_timeout_ms;
        transfer_timeout_ms_ = transfer_timeout_ms;

        curl_ = curl_easy_init();

        if(NULL == curl_) {
            return EASY_CRUL_API_EXECUTION_ERROR;
        }

        // keep cache to reconnect quickly
        curl_easy_setopt(curl_, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 60 * 72);

        int ret = 0;
        ret = this->Connect();

        if(0 != ret) {
            return CONNECT_ERROR;
        }

        return SUCCESS;
    }

    int Request(const std::string &url, const std::string &response_end_flag,
                std::string &response, long &port) {

        long long timeleft = transfer_timeout_ms_;
        timeval start, now;
        gettimeofday(&start, NULL);

        size_t len = 0;
        std::string request;
        request = "GET " + url + " HTTP/1.0\r\n"
                  + "Host: " + host_ + "\r\n"
                  + "Connection: Keep-Alive\r\n\r\n";

        int ret = SUCCESS;
        /* wait for the socket to become ready for sending */
        ret = this->WaitOnSocket(sockfd_, 0, timeleft);

        if(SUCCESS != ret) {
            this->Connect(1); //force
            return RETRY_REQUEST_AGAIN;
        }

        /* Send the request. Real applications should check the iolen
         * to see if all the request has been sent */
        ret = curl_easy_send(curl_, request.c_str(), request.length(), &len);

        if(CURLE_OK != ret) {
            // mostly because of out of connection and reconnect it;
            this->Connect();
            return RECV_FAILED;
        }

        if(request.length() != len) {
            return SEND_DATA_ERROR;
        }

        // read the response
        response.clear();
        int last_length = 0;
        gettimeofday(&now, NULL);
        timeleft -= ((now.tv_sec - start.tv_sec) * 1000
                     + (now.tv_usec - start.tv_usec) / 1000);
        if(timeleft < 0) {
            ret = TIMEOUT;
            return ret;
        }

        while(true) {
            gettimeofday(&start, NULL);
            ret =  this->WaitOnSocket(sockfd_, 1, timeleft);

            if(SUCCESS != ret) {
                this->Connect();
                break;
            }

            ret = curl_easy_recv(curl_, recv_buffer_, sizeof(recv_buffer_), &len);

            if(CURLE_OK != ret) {
                this->Connect();
                break;
            }

            response.append(recv_buffer_, len);

            size_t pos = 0;

            if(response_end_flag.empty()) {
                // not set end flag, so will wait unit all no data any more
                continue;
            }

            if(last_length >= response_end_flag.length()) {
                // this is a optimization step: for find whether to find the response end flag this time.
                pos = last_length - response_end_flag.length() + 1;
            }

            if(response.find(response_end_flag, pos) != std::string::npos) {
                // the http package is already finished; and begin to run
                break;
            }

            last_length = response.length();
            gettimeofday(&now, NULL);
            timeleft -= ((now.tv_sec - start.tv_sec) * 1000
                         + (now.tv_usec - start.tv_usec) / 1000);
            if(timeleft < 0) {
                ret = TIMEOUT;
                break;
            }
        }

        curl_easy_getinfo(curl_, CURLINFO_LOCAL_PORT , &port);

        return ret;
    }

    int Release() {
        curl_easy_cleanup(curl_);
        return SUCCESS;
    }

  private:
    int WaitOnSocket(curl_socket_t sockfd, int for_recv, long timeout_ms) {
        struct timeval tv;
        fd_set infd, outfd, errfd;
        int ret;

        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        FD_ZERO(&infd);
        FD_ZERO(&outfd);
        FD_ZERO(&errfd);

        FD_SET(sockfd, &errfd);  /* always check for error */

        if(for_recv) {
            FD_SET(sockfd, &infd);
        } else {
            FD_SET(sockfd, &outfd);
        }

        /* select() returns the number of signalled sockets or -1 */
        ret = select(sockfd + 1, &infd, &outfd, &errfd, &tv);

        if(ret < 0) {
            return SELECT_ERROR;
        } else if(ret == 0) {
            if(for_recv) {
                return RECV_TIMEOUT_ERROR;
            } else {
                return SEND_TIMEOUT_ERROR;
            }
        } else {
            return SUCCESS;
        }
    }

    int Connect(int flag = 0) {
        int ret = 0;

        struct tcp_info info;
        int len = sizeof(info);
        getsockopt(sockfd_, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
        if((info.tcpi_state == TCP_ESTABLISHED)) {
            printf("socket connected,reuse again\n");
            return 0;
        } else {
            printf("socket disconnected,do connection\n");
            //return 0;
        }
        /*
            int optval;// optlen = sizeof(int);
            socklen_t optlen;
            getsockopt (sockfd_, SOL_SOCKET, SO_ERROR, (char *) &optval, &optlen);

            if (!flag && optval == 0) {
                cout << "try connect" << endl;
                return 0;
            }

            cout << "real connect" << endl;
        */


        // close previous socket
        /*
            if (sockfd_) {
                close (sockfd_);
            }
        */

        long long timeleft = connect_timeout_ms_;
        timeval start, now;
        gettimeofday(&start, NULL);

        curl_easy_setopt(curl_, CURLOPT_URL, uri_.c_str());
        /* Do not do the transfer - only connect to host */
        curl_easy_setopt(curl_, CURLOPT_CONNECT_ONLY, 1L);
        ret = curl_easy_perform(curl_);

        if(0 != ret) {
            return EASY_CRUL_API_EXECUTION_ERROR;
        }

        long sockextr = 0;
        ret = curl_easy_getinfo(curl_, CURLINFO_LASTSOCKET, &sockextr);

        if(0 != ret) {
            return EASY_CRUL_API_EXECUTION_ERROR;
        }

        curl_easy_setopt(curl_, CURLOPT_CONNECT_ONLY, 0L);
        sockfd_ = sockextr;
        int optval = 32 * 1024; // 32k
        setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF , (void*) &optval, sizeof(int));

        timeleft -= ((now.tv_sec - start.tv_sec) * 1000
                     + (now.tv_usec - start.tv_usec) / 1000);
        if(timeleft < 0) {
            ret = TIMEOUT;
            return ret;
        }

        return SUCCESS;
    }

    CURL *curl_;
    curl_socket_t sockfd_;
    std::string host_;
    std::string uri_;
    int connect_timeout_ms_;
    int transfer_timeout_ms_;
    char recv_buffer_[10240];

    LiveCurl(const LiveCurl &);
    LiveCurl &operator= (const LiveCurl &);
};

#endif  // _LIVE_CURL_H_
