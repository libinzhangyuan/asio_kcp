#ifndef _ASIO_KCP_CLIENT_WRAP__
#define _ASIO_KCP_CLIENT_WRAP__


#include "kcp_client.hpp"

namespace asio_kcp {
/*
 * kcp_client_wrap is a facade of kcp_client.
 * This facade is easy to use. You do not need use it in an event-driven framework such as boost.asio, libevent, cocos2d, or implement by your own.
 * Please using kcp_client directly if you coding in an event-driven framework. That's more effective.
 * kcp_client_wrap will create a work thread that control the udp packet sending and recving.
*/

/*  sync using
 *   kcp_client_wrap c;
 *   c.connect   // this will block until connect succeed or failed.
 *   while (true)
 *   {
 *      do your things && if some msg need send to server
 *                            c.send_msg
 *      events = c.grab_events
 *      handle_events(events)  // your func
 *      millisecond_sleep(1) if you want.
 *   }
*/
// You can async the connection stage or event handle stage.
//
/* async the connect stage
 *  kcp_client_wrap c;
 *  c.connect_async
 *  while (c.connect_result() == 1)
 *      millisecond_sleep 1
 */

/* async connect with callback_func
 *  kcp_client_wrap c;
 *  c.set_connect_event_callback_func()
 *  c.connect_async
 *  ...
 *  kcp_client_wrap will call connect_event_callback_func in another thread.   note: you should making connect_event_callback_func multithread safe.
 */

/* async event handle
 *  kcp_client_wrap c;
 *  c.set_event_callback
 *  c.connect  or using  c.connect_async mode
 *  while (true)
 *  {
 *      do your things && if some msg need send to server
 *                            c.send_msg
 *      millisecond_sleep(1) if you want.
 *  }
 *  ...
 *  kcp_client_wrap will call event_call_back_func in another thread. note: you should making event_call_back_func multithread safe.
*/
class kcp_client_wrap
{
public:
    kcp_client_wrap(void);
    ~kcp_client_wrap(void);

    void set_event_callback(const client_event_callback_t& event_callback_func, void* var);

    // Sync connect. This function will block until connect succeed or failed.
    // we use system giving local port from system if udp_port_bind == 0
    // return 0 if connect succeed.
    // return < 0 (KCP_ERR_XXX) when some error happen.
    int connect(int udp_port_bind, const std::string& server_ip, const int server_port);



    // Async connect
    //
    // we use system giving local port from system if udp_port_bind == 0
    // return 0 means all is ok.  return KCP_ERR_XXX if some error happen.
    // kcp_client_wrap will call event_callback_func when connect succeed or failed if you set_event_callback.
    //   or you need call c.connect_result() repeatly until it return < 1
    int connect_async(int udp_port_bind, const std::string& server_ip, const int server_port);

    // 0: connect succeed,  1: need waiting connect end,   <0: connect fail, and it's error code.
    int connect_result(void) const {return connect_result_;}

    //
    // end of Async connect


    // user level send msg.
    void send_msg(const std::string& msg);

    void stop();

private:

    // Node: changed!  Would not need call this start_workthread() function.
    // connect() and connect_async() function will call start_workthread for your.
    void start_workthread(void);


    static void client_event_callback_func(kcp_conv_t conv, eEventType event_type, const std::string& msg, void* var);
    void handle_client_event_callback(kcp_conv_t conv, eEventType event_type, const std::string& msg);

    int do_asio_kcp_connect_loop(void);

    static void* workthread_loop(void* _this);
    void do_workthread_loop(void);

private:
    kcp_client kcp_client_;
    int connect_result_; // 0: connect succeed,  1: need waiting connect end,   <0: connect fail, and it's error code.
    client_event_callback_t* pevent_func_;
    void* event_func_var_;


    pthread_t workthread_;
    volatile bool workthread_want_stop_;
    volatile bool workthread_stopped_; // indicate that the workthread stopped already.
    volatile bool workthread_start_;

    uint64_t kcp_last_update_clock_;
};

} // namespace asio_kcp

#endif // _ASIO_KCP_CLIENT_WRAP__
