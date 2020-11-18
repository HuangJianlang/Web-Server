
#ifndef WEB_SERVER_EVENT_BASE_H
#define WEB_SERVER_EVENT_BASE_H

#include "handy_impl.h"
#include "poller.h"

namespace my_handy{

    using TcpConnPtr = std::shared_ptr<TcpConn>;
    using TcpServerPtr = std::shared_ptr<TcpServer>;
    using TcpCallBack = std::function<void(const TcpConnPtr &)>;
    using MsgCallBack = std::function<void(const TcpConnPtr &, Slice msg)>;

    class EventBases : private non_copyable {
    public:
        //memory allocator
        virtual EventBases *allocBase() = 0 ;
    };

    //事件派发器 可管理定时器 连接 超时连接
    class EventBase : public EventBases {
    public:
        EventBase(int taskCapacity = 0);
        ~EventBase();
        //处理已到期的事件, waitMs 表示若无当前需要处理的任务 需要等待的事件
        void loop_once(int waitMs);
        //进入事件处理循环
        void loop();

        bool cancel(TimerId timerId);

        TimerId  runAt(int64_t milli, const Task& task, int64_t interval=0){
            return runAt(milli, Task(task), interval);
        }

        TimerId runAt(int64_t milli, Task&& task, int64_t interval=0);

        TimerId runAfter(int64_t milli, const Task& task, int64_t interval = 0){
            return runAt(util::timeMilli()+milli, Task(task), interval);
        }

        TimerId runAfter(int64_t milli, Task&& task, int64_t interval=0){
            return runAt(util::timeMilli()+milli, std::move(task), interval);
        }

        EventBase &exit();
        bool exited();
        void wakeup();
        void safeCall(Task&& task);
        void safeCall(const Task& task){
            //why need call ctor?
            safeCall(Task(task));
        }

        virtual EventBase* allocBase(){
            return this;
        }

        std::unique_ptr<EventsImpl> impl_;
    };

    class MultiBase : public EventBases {
    public:
        MultiBase(int sz) : id_(0), base_(sz){
        }

        virtual EventBase* allocBase() {
            int c = id_++;
            return &bases_[c % bases_.size()];
        }

        void loop();

        MultiBase &exit(){
            for (auto &b : bases_){
                b.exit();
            }
            return *this;
        }

    private:
        std::atomic<int> id_;
        std::vector<EventBase> bases_;
    };
}



#endif //WEB_SERVER_EVENT_BASE_H
