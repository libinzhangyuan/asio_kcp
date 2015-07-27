#ifndef _ASIO_KCP_KCP_CLIENT_
#define _ASIO_KCP_KCP_CLIENT_

#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>


#include "threadsafe_queue_mutex.hpp"

struct IKCPCB;
typedef struct IKCPCB ikcpcb;

// indicate a converse between a client and connection_obj between server.
typedef uint32_t kcp_conv_t;

#define MAX_MSG_SIZE 1024 * 10
#define KCP_UPDATE_INTERVAL 5 // milliseconds
#define KCP_RESEND_CONNECT_MSG_INTERVAL 500 // milliseconds
#define KCP_CONNECT_TIMEOUT_TIME 5000 // milliseconds

#define KCP_ERR_ALREADY_CONNECTED       -2001
#define KCP_ERR_ADDRESS_INVALID         -2002
#define KCP_ERR_CREATE_SOCKET_FAIL      -2003
#define KCP_ERR_SET_NON_BLOCK_FAIL      -2004

#define KCP_ERR_CONNECT_FUNC_FAIL       -2010
#define KCP_ERR_KCP_CONNECT_TIMEOUT     -2011

#define KCP_CONNECT_TIMEOUT_MSG "connect timeout"

namespace asio_kcp {

enum eEventType
{
    eConnect,
    eConnectFailed,
    eDisconnect,
    eRcvMsg,

    eCountOfEventType
};
typedef void(client_event_callback_t)(kcp_conv_t /*conv*/, eEventType /*event_type*/, const std::string& /*msg*/, void* /*var*/);


/*
 * using asio_kcp_client in a event-driven framework. You should hook a timer for calling the kcp_client.update()
 * all code running in your coding thread. This is no other thread.
 *
 * You can set event_callback_func or not.

 * Using event_callback_func:
 *   in class header define a member client_;
 *     kcp_client client_;
 *
 *   in An event handle:
 *     client_.set_event_callback()
 *     hook 5milliseconds_timer_handle in your event-driven framework
 *     client_.connect_async()
 *
 *   in 5milliseconds_timer_handle
 *     client_.update()
 *     client_.update() will call event_callback_func back (in same thread) when connect succeed or failed
 *
 *   aftering the success of connection. You can call c.send_msg in your code
 *   client_.update() in 5milliseconds_timer_handle will call event_callback_func back (in same thread) when recved some msg or some error happenned (disconnect)
*/

/* Not using event_callback_func
 *   in class header define a member client_;
 *     kcp_client client_;
 *
 *   in An event handle:
 *     hook 5milliseconds_timer_handle in your event-driven framework
 *     client_.connect_async()
 *
 *   in 5milliseconds_timer_handle
 *     client_.update()
 *     events = client_.grab_events
 *     events.for_each  handle event
 *
 *   aftering the success of connection. You can call c.send_msg in your code
 *   client_.update() in 5milliseconds_timer_handle will call event_callback_func back (in same thread) when recved some msg or some error happenned (disconnect)
*/


/*
 * in class header define a member client_;
 *  kcp_client client_;
 *
 * in An event handle:
 *  hook 5milliseconds_timer_handle in your event-driven framework
 *  client_.connect_async()
 *
 * in 5milliseconds_timer_handle
 *  client_.update()
 *  events = client_.grab_events
 *  events.for_each  handle event
 *
 * 
 *  waiting event_call_back_func be called. And tell you connect succeed or failed.
 *
 *  aftering the success of connection. You can call c.send_msg in your code
 */
/*
 * using event_callback_func.
 * in 5milliseconds_timer_hook
 * c.update()
   kcp_client will call event_callback_func in same thread when recved some msg or some error happenned (disconnect)
*/

class kcp_client
{
public:
    kcp_client(void);
    ~kcp_client(void);

    void set_event_callback(const client_event_callback_t& event_callback_func, void* var);

    // we use system giving local port from system if udp_port_bind == 0
    // return KCP_ERR_XXX if some error happen.
    // kcp_client will call event_callback_func when connect succeed or failed.
    int connect_async(int udp_port_bind, const std::string& server_ip, const int server_port);

    void update();

    // user level send msg.
    // this func is multithread safe.
    void send_msg(const std::string& msg);

    // Stop connections.
    // this func is multithread safe.
    void stop();

private:
    kcp_client(const kcp_client&);

    void init_kcp(kcp_conv_t conv);

private:
    // return 0 if connect succeed.
    // return < 0 (KCP_ERR_XXX) when some error happen.
    int init_udp_connect(void);

    bool connect_timeout(uint64_t cur_clock) const;
    bool need_send_connect_packet(uint64_t cur_clock) const;
    void do_asio_kcp_connect(uint64_t cur_clock);


    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);
    void send_udp_package(const char *buf, int len);
    void do_send_connect_packet(uint64_t cur_clock);


    void do_recv_udp_packet_in_loop(void);
    void do_send_msg_in_queue(void);
    void handle_udp_packet(const std::string& udp_packet);
    void try_recv_connect_back_packet(void);

    std::string recv_udp_package_from_kcp(void);

    bool in_connect_stage_;
    uint64_t connect_start_time_;
    uint64_t last_send_connect_msg_time_;
    bool connect_succeed_;



    client_event_callback_t* pevent_func_;
    void* event_callback_var_;

    threadsafe_queue_mutex<std::string> send_msg_queue_;

    int udp_port_bind_;
    std::string server_ip_;
    int server_port_;
    int udp_socket_;
    struct sockaddr_in servaddr_;
    char udp_data_[1024 * 4];

    ikcpcb* p_kcp_; // --own
};

} // namespace asio_kcp

#endif // _ASIO_KCP_KCP_CLIENT_
