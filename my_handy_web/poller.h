#ifndef WEB_SERVER_POLLER_H
#define WEB_SERVER_POLLER_H

#include <assert.h>
#include <poll.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <atomic>
#include <map>

#include "util.h"

//poller class encapsulation of poll
//maybe I can modified it as a base class to decouple OS system call
namespace my_handy{

    const int K_MAX_EVENTS = 2000;
    const int K_READ_EVENT = POLLIN;
    const int K_WRITE_EVENT = POLLOUT;

    class PollerBase : private non_copyable{
    private:
        int64_t id_;
        int lastActive_;

    public:
        PollerBase() : lastActive_(-1){
            static std::atomic<int64_t> id(0); //atomic operation will atomic?
            id_ = ++id;
        }

        //pure virtual func
        virtual void addChannel(Channel *ch) = 0;
        virtual void removeChannel(Channel *ch) = 0;
        virtual void updateChannel(Channel *ch) = 0;
        virtual void loop_once(int waitMs) = 0;
    };

    //factory mode?
    PollerBase* createPoller();
}



#endif //WEB_SERVER_POLLER_H
