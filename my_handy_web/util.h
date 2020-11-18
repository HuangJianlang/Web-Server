#ifndef WEB_SERVER_UTIL_H
#define WEB_SERVER_UTIL_H

#include <stdlib.h>
#include <string.h>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace my_handy{
    //Base class for deleting copy ctor and operator=
    class non_copyable {
    protected:
        non_copyable() = default;
        virtual ~non_copyable() = default;

        non_copyable(const non_copyable&) = delete ;
        non_copyable& operator=(const non_copyable&) = delete;
    };

    class ExitCaller : private non_copyable{
    private:
        std::function<void()> functor_;

    public:
        ExitCaller(std::function<void()>&& func)
            : functor_(std::move(func)){

        }

        ~ExitCaller(){
            functor_();
        }
    };

    class util {

    };
};



#endif //WEB_SERVER_UTIL_H
