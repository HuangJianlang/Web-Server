#include <fcntl.h>
#include <errno.h>
#include "conn.h"
#include "event_base.h"
#include "logging.h"
#include "util.h"
#include "poller.h"

#ifdef OS_LINUX
#include <sys/epoll.h>
#elif defined(OS_MACOSX)
#include <sys/event.h>
#else
#error "platform unsupported"
#endif

namespace my_handy{

    class PollerEpoll : public PollerBase{
    private:
        int fd_;
        std::set<Channel*> liveChannels_;
        struct epoll_event activeEvents_[K_MAX_EVENTS];

    public:
        PollerEpoll();
        ~PollerEpoll();
        virtual void addChannel(Channel *ch) override;
        virtual void removeChannel(Channel *ch) override;
        virtual void updateChannel(Channel *ch) override;
        virtual void loop_once(int waitMs) override;
    };

    PollerBase* createPoller(){
        return new PollerEpoll();
    }

    PollerEpoll::PollerEpoll() {
        fd_ = epoll_create(EPOLL_CLOEXEC);
        //logging
        fatalif(fd_ < 0, "epoll_create error %d %s", errno, strerror(errno));
        info("poller epoll %d created", fd_);
    }

    PollerEpoll::~PollerEpoll() {
        info("destroying poller %d", fd_);
        while (liveChannels_.size()){
            (*liveChannels_.begin())->close();
        }
        ::close(fd_);
        info("poller %d destroyed", fd_);
    }
}