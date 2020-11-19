#ifndef WEB_SERVER_UDP_H
#define WEB_SERVER_UDP_H

#include "event_base.h"

namespace my_handy{
    class UdpServer;
    class UdpConn;

    using UdpConnPtr = std::shared_ptr<UdpConn>;
    using UdpServerPtr = std::shared_ptr<UdpServer>;
    using UdpCallBack = std::function<void(const UdpConnPtr&, Buffer)>;
    using UdpServerCallBack = std::function<void(const UdpServerPtr&, Buffer, Ip4Addr)>;
    const int K_UDP_PACKET_SIZE = 4096;

    class UdpServer : private non_copyable, public std::enable_shared_from_this<UdpServer> {
    private:
        EventBase* base_;
        EventBases* bases_;
        Ip4Addr addr_;
        Channel* channel_;
        UdpServerCallBack msg_callback_;

    public:
        static UdpServerPtr startServer(EventBases *bases, const std::string& host, unsigned short port, bool reusePort = false);


        UdpServer(EventBases* bases);

        ~UdpServer(){
            delete channel_;
        }

        int bind(const std::string& host, unsigned short port, bool reusePort = false);

        Ip4Addr getAddr(){
            return addr_;
        }

        EventBase* getBase(){
            return base_;
        }

        void sendTo(const char* buf, size_t len, Ip4Addr addr);
        void sendTo(const std::string& str, Ip4Addr addr){
            sendTo(str.data(), str.size(), addr);
        }
        void sendTo(const char* buf, Ip4Addr addr){
            sendTo(buf, strlen(buf), addr);
        }

        void onMsg(const UdpServerCallBack& callBack) {
            msg_callback_ = callBack;
        }
    };

    class UdpConn : public std::enable_shared_from_this<UdpConn>, private non_copyable {
    private:
        EventBase* base_;
        Channel* channel_;
        Ip4Addr local_, peer_;
        AutoContext ctx_;
        std::string destHost_;
        int destPort_;
        UdpCallBack callBack_;

        void handleRead(const UdpConnPtr&);

    public:
        static UdpConnPtr createConnection(EventBase* base, const std::string& host, unsigned short port);

        UdpConn();
        virtual ~UdpConn(){
            close();
        }

        template <class T>
        T& context(){
            return ctx_.context<T>();
        }

        EventBase* getBase(){
            return base_;
        }

        Channel* getChannel(){
            return channel_;
        }

        void send(const char* buf, size_t len);
        void send(const std::string& s){
            send(s.data(), s.size());
        }
        void send(const char* s){
            send(s, strlen(s));
        }
        void onMsg(const UdpCallBack& callBack){
            callBack_ = callBack;
        }

        void close();

        std::string str(){
            return peer_.toString();
        }

        using RetMsgUdpCallBack = std::function<void(const UdpServerPtr&, const std::string&, Ip4Addr)>;

        //半同步半反应堆模式
        //todo: 半同步半反应堆模式
    };
}



#endif //WEB_SERVER_UDP_H
