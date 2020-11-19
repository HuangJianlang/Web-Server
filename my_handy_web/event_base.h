
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
        MultiBase(int sz) : id_(0), bases_(sz){
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

    //作用是将事件与fd绑定在一起，能够对事件进行操作(读写允许, callback设置)管理
    class Channel : private non_copyable {
    public:
        Channel(EventBase* base, int fd, int events);
        ~Channel();

        EventBase* getBase(){
            return base_;
        }

        int getFd(){
            return fd_;
        }

        int64_t getId(){
            return id_;
        }

        short getEvents(){
            return events_;
        }
        void close();

        //配置event handle
        void onRead(const Task& read_callback){
            read_callback_ = read_callback;
        }

        void onWrite(const Task& write_callback){
            write_callback_ = write_callback;
        }

        void onRead(Task&& read_callback){
            read_callback_ = std::move(read_callback);
        }

        void onWrite(Task&& write_callback){
            write_callback_ = std::move(write_callback);
        }

        //启动读写监听
        void enableRead(bool enable);
        void enableWrite(bool enable);
        void enableRW(bool readEnable, bool writeEnable);

        bool isReadEnable();
        bool isWriteEnable();

        void handleRead(){
            read_callback_();
        }

        void handleWrite(){
            write_callback_();
        }

    protected:
        int fd_;
        short events_;
        int64_t id_;

        //这里为什么不用EventBases?
        EventBase* base_;
        PollerBase* poller_;
        std::function<void()> read_callback_, write_callback_, error_callback;
    };
}




#endif //WEB_SERVER_EVENT_BASE_H
