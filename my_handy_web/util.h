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
        static std::string format(const char* fmt, ...);
        static int64_t timeMicro();
        static int64_t timeMilli(){
            return timeMicro() / 1000;
        }
        static int64_t steadyMicro();
        static int64_t steadyMilli() {
            return steadyMicro() / 1000;
        }

        static std::string readableTime(time_t t);

        static int64_t atoi(const char *b, const char *e) {
            return strtol(b, (char **) &e, 10);
        }

        static int64_t atoi2(const char *b, const char *e) {
            char **ne = (char **) &e;
            int64_t v = strtol(b, ne, 10);
            return ne == (char **) &e ? v : -1;
        }

        static int64_t atoi(const char *b) {
            return atoi(b, b + strlen(b));
        }

        static int addFdFlag(int fd, int flag);

    };
};



#endif //WEB_SERVER_UTIL_H
