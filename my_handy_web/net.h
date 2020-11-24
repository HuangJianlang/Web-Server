//
// Created by Jianlang Huang on 11/22/20.
//

#ifndef WEB_SERVER_NET_H
#define WEB_SERVER_NET_H

#include <netinet/in.h>
#include <string.h>
#include <algorithm>
#include <string>
#include "port_posix.h"
#include "slice.h"

namespace my_handy{
    class net {
    public:
        template <class T>
        static T hton(T v) {
            return port::htobe(v);
        }
        template <class T>
        static T ntoh(T v) {
            return port::htobe(v);
        }
        static int setNonBlock(int fd, bool value = true);
        static int setReuseAddr(int fd, bool value = true);
        static int setReusePort(int fd, bool value = true);
        static int setNoDelay(int fd, bool value = true);
    };

    class Ip4Addr {
    private:
        struct sockaddr_in addr_;

    public:
        Ip4Addr(const std::string &host, unsigned short port);
        Ip4Addr(unsigned short port) : Ip4Addr("", port){};
        Ip4Addr(const struct sockaddr_in addr) : addr_(addr){};

        std::string toString() const;
        std::string ip() const;
        unsigned short port() const;
        unsigned int ipInt() const;


        bool isIpValid() const;

        struct sockaddr_in &getAddr(){
            return addr_;
        }

        static std::string hostToIp(const std::string &host){
            Ip4Addr addr(host, 0);
            return addr.ip();
        }

    };

    class Buffer {
    private:
        char* buf_;
        
    };
}



#endif //WEB_SERVER_NET_H
