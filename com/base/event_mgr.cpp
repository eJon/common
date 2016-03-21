#include <errno.h>
#include "log.h"
#include "event_mgr.h"

using namespace ms::log;

CEventMgr::CEventMgr() {
    thread_num_ = 0;
    event_list_ = NULL;
    poll_event_ = NULL;
}

CEventMgr::~CEventMgr() {
    UnInit();
}

int CEventMgr::Init(int thread_num) {
    thread_num_ = thread_num;
    event_list_ = new CEvent[thread_num_];
    if(event_list_ == NULL) {
        return ENUM_MS_EVENTMGR_MEMOVER;
    }
    for(int i = 0; i < thread_num_; i++) {
        if(event_list_[i].Init()) {
            return ENUM_MS_EVENTMGR_PIPE_INIT_ERR;
        }
    }
    poll_event_ = new struct pollfd[thread_num_];
    if(poll_event_ == NULL) {
        return ENUM_MS_EVENTMGR_MEMOVER;
    }
    filter_ = new int[thread_num];
    if(filter_ == NULL) {
        return ENUM_MS_EVENTMGR_MEMOVER;
    }
    return 0;
}

void CEventMgr::UnInit(void) {
    thread_num_ = 0;
    if(event_list_ != NULL) {
        delete[] event_list_;
        event_list_ = NULL;
    }
    if(poll_event_ != NULL) {
        delete[] poll_event_;
        poll_event_ = NULL;
    }
    if(filter_ != NULL) {
        delete[] filter_;
        filter_ = NULL;
    }
}

int CEventMgr::Reset(int thread_num) {
    UnInit();
    return Init(thread_num);
}

int CEventMgr::SetEvent(int thread_idx) {
    if((thread_idx < 0) || (thread_idx > (thread_num_ - 1))) {
        return ENUM_MS_EVENTMGR_PARAM_ERR;
    }
    if(write(event_list_[thread_idx].w_pipe_, "", 1) != 1) {
        ERROR(LOGROOT, "CEventMgr::SetEvent write pipe error");
        return ENUM_MS_EVENTMGR_PIPE_W_ERR;
    }
    return ENUM_MS_EVENTMGR_OK;
}
int CEventMgr::ResetEvent(int thread_idx) {
    if((thread_idx < 0) || (thread_idx > (thread_num_ - 1))) {
        return ENUM_MS_EVENTMGR_PARAM_ERR;
    }
    char buf[1];
    if(read(event_list_[thread_idx].r_pipe_, buf, 1) != 1) {
        ERROR(LOGROOT, "CEventMgr::ResetEvent read pipe error");
        return ENUM_MS_EVENTMGR_PIPE_R_ERR;
    }
    return ENUM_MS_EVENTMGR_OK;
}
int CEventMgr::WaitEvent(int thread_idx, int iMilliseconds) {
    if((thread_idx < 0) || (thread_idx > (thread_num_ - 1))) {
        return ENUM_MS_EVENTMGR_PARAM_ERR;
    }
    int res = 0;
    struct pollfd fds[1];
    fds[0].fd = event_list_[thread_idx].r_pipe_;
    fds[0].events = POLLIN;
    res = poll(fds, 1, iMilliseconds);
    if(res < 0) {
        ERROR(LOGROOT, "CEventMgr::WaitEvent System error[=%d]!", errno);
        return ENUM_MS_EVENTMGR_POLL_ERR;
    }
    if(res == 0) {
        ERROR(LOGROOT, "CEventMgr::WaitEvent time out");
        return ENUM_MS_EVENTMGR_POLL_TIMEOUT;
    }
    if(fds[0].events & (POLLHUP | POLLERR)) {
        ERROR(LOGROOT, "CEventMgr::WaitEvent pipe device error");
        return ENUM_MS_EVENTMGR_PIPE_R_ERR;
    }
    if(fds[0].events & POLLIN) {
        return ResetEvent(thread_idx);
    }
    return ENUM_MS_EVENTMGR_OK;
}

int CEventMgr::WaitMultipleEvent(int nCount, int *filter_src, int iMilliseconds) {
    if((nCount <= 0) || (nCount > thread_num_) || (filter_src == NULL)) {
        return ENUM_MS_EVENTMGR_PARAM_ERR;
    }
    while(1) {
        int event_num = 0;
        for(int i = 0; i < nCount; i++) {
            if(filter_src[i] == 1) {
                poll_event_[event_num].fd = event_list_[i].r_pipe_;
                poll_event_[event_num].events = POLLIN;
                filter_[event_num] = i;
                event_num++;
            }
        }
        if(event_num == 0) {
            return ENUM_MS_EVENTMGR_OK;
        }
        int res = poll(poll_event_, event_num, iMilliseconds);
        if(res < 0) {
            ERROR(LOGROOT, "CEventMgr::WaitEvent System error[=%d]!", errno);
            return ENUM_MS_EVENTMGR_POLL_ERR;
        }
        if(res == 0) {
            ERROR(LOGROOT, "CEventMgr::WaitEvent time out");
            return ENUM_MS_EVENTMGR_POLL_TIMEOUT;
        }
        for(int i = 0; i < event_num; i++) {
            if(poll_event_[i].events & (POLLHUP | POLLERR)) {
                ERROR(LOGROOT, "CEventMgr::WaitEvent pipe device closed!");
                return ENUM_MS_EVENTMGR_PIPE_DEV_ERR;
            }
            if(poll_event_[i].events & POLLIN) {
                int ret = ResetEvent(filter_[i]);
                if(ret) {
                    return ret;
                }
                filter_src[filter_[i]] = 0;
            }
        }
    }
    return ENUM_MS_EVENTMGR_OK;
}
