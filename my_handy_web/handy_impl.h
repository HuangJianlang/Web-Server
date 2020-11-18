//
// Created by Roy on 11/17/20.
//

#include <unistd.h>
#include <memory>
#include <set>
#include <utility>
#include "codec.h"
#include "logging.h"
#include "net.h"
#include "threads.h"
#include "util.h"

namespace my_handy{
    class Channel;
    class TcpConn;
    class TcpServer;
    class IdleIdImp;
    class EventsImp;
    class EventBase;

    using IdleId = std::unique_ptr<IdleIdImp>;
    using TimerId = std::pair<int64_t, int64_t>;

    class AutoContext : public non_copyable {
    public:
        void* ctx;
        Task ctxDel;
        AutoContext() : ctx(0){

        }

        template<typename T>
        T& context(){
            if (ctx == nullptr){
                ctx = new T();
                ctxDel = [this](){
                    delete (T*) ctx;
                };
            }
            //ctx is void ptr -> need T -> cast to T* -> become T ptr
            return *(T*) ctx;
        }

        ~AutoContext(){
            if (ctx){
                ctxDel();
            }
        }
    };
}