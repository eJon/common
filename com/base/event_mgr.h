#ifndef MATCHSERVER_FEED_EVENT_MGR_H
#define MATCHSERVER_FEED_EVENT_MGR_H

#include <poll.h>
#include <unistd.h>
#include <string.h>

enum ENUM_MS_EVENTMGR_ERROR {
    ENUM_MS_EVENTMGR_OK = 0,
    ENUM_MS_EVENTMGR_MEMOVER,
    ENUM_MS_EVENTMGR_PARAM_ERR,
    ENUM_MS_EVENTMGR_PIPE_W_ERR,
    ENUM_MS_EVENTMGR_PIPE_R_ERR,
    ENUM_MS_EVENTMGR_POLL_ERR,
    ENUM_MS_EVENTMGR_POLL_TIMEOUT,
    ENUM_MS_EVENTMGR_PIPE_DEV_ERR,
    ENUM_MS_EVENTMGR_PIPE_INIT_ERR,
};

class CEventMgr {
  public:
    CEventMgr();
    ~CEventMgr();
  public:
    class CEvent {
      public:
        CEvent() {
            memset(this, 0, sizeof(CEvent));
        }
        ~CEvent() {
            UnInit();
        }
      public:
        int Init(void) {
            int fds[2];
            if(pipe(fds)) {
                return 1; //pipe open failed
            }
            r_pipe_ = fds[0];
            w_pipe_ = fds[1];
            return 0;
        }
        void UnInit(void) {
            if(r_pipe_ != 0) {
                close(r_pipe_);
                r_pipe_ = 0;
            }
            if(w_pipe_ != 0) {
                close(w_pipe_);
                w_pipe_ = 0;
            }
        }
      public:
        int r_pipe_;
        int w_pipe_;
    };
  public:
    int Init(int thread_num);
    void UnInit(void);
    int Reset(int thread_num);
  public:
    int SetEvent(int thread_idx);
    int WaitEvent(int thread_idx, int iMilliseconds = -1);
    int WaitMultipleEvent(int nCount, int *thread_idxs,  int iMilliseconds = -1);
  private:
    int ResetEvent(int thread_idx);
  private:
    int thread_num_;
    CEvent *event_list_;
    struct pollfd *poll_event_;
    int *filter_;
};

#endif //MATCHSERVER_FEED_EVENT_MGR_H

