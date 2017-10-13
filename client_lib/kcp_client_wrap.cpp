#include <iostream>

#include "kcp_client_wrap.hpp"
#include "kcp_client_util.h"

namespace asio_kcp {

kcp_client_wrap::kcp_client_wrap(void) :
    connect_result_(1),
    pconnect_event_func_(NULL),
    connect_event_func_var_(NULL),
    pevent_func_(NULL),
    event_func_var_(NULL),
    workthread_(0),
    workthread_want_stop_(false),
    workthread_stopped_(false),
    workthread_start_(false),
    kcp_last_update_clock_(0)
{
    kcp_client_.set_event_callback(client_event_callback_func, (void*)this);
}

kcp_client_wrap::~kcp_client_wrap(void)
{
    stop();
}

void kcp_client_wrap::set_event_callback(const client_event_callback_t& event_callback_func, void* var)
{
    pevent_func_ = &event_callback_func;
    event_func_var_ = var;
}

void kcp_client_wrap::client_event_callback_func(kcp_conv_t conv, eEventType event_type, const std::string& msg, void* var)
{
    ((kcp_client_wrap*)var)->handle_client_event_callback(conv, event_type, msg);
}

void kcp_client_wrap::handle_client_event_callback(kcp_conv_t conv, eEventType event_type, const std::string& msg)
{
    std::cout << "kcp_client_wrap::handle_client_event_callback event_type: " << event_type << std::endl;
    switch (event_type)
    {
        case eConnect:
            connect_result_ = 0;
            if (pconnect_event_func_)
                (*pconnect_event_func_)(conv, event_type, msg, event_func_var_);
            break;
        case eConnectFailed:
            // if msg == KCP_CONNECT_TIMEOUT_MSG
            connect_result_ = KCP_ERR_KCP_CONNECT_TIMEOUT;
            if (pconnect_event_func_)
                (*pconnect_event_func_)(conv, event_type, msg, event_func_var_);
            break;
        case eRcvMsg:
        case eDisconnect:
            if (pevent_func_)
                (*pevent_func_)(conv, event_type, msg, event_func_var_);
            break;
        default:
            ; // do nothing
    }
}

int kcp_client_wrap::connect(int udp_port_bind, const std::string& server_ip, const int server_port)
{
    int ret_connect_async = kcp_client_.connect_async(udp_port_bind, server_ip, server_port);
    if (ret_connect_async < 0)
        return ret_connect_async;

    return do_asio_kcp_connect_loop();
}

int kcp_client_wrap::do_asio_kcp_connect_loop(void)
{
    while (true)
    {
        if (connect_result_ != 1)
            return connect_result_;
        kcp_client_.update();
        millisecond_sleep(KCP_UPDATE_INTERVAL);
    }
}

void kcp_client_wrap::start_workthread(void)
{
    int ret = pthread_create(&workthread_, NULL, &kcp_client_wrap::workthread_loop, (void*)this);
    if (ret != 0)
    {
        std::cerr << "start_workthread pthread_create error: " << ret << std::endl;
        return;
    }
}

void* kcp_client_wrap::workthread_loop(void* _this)
{
    ((kcp_client_wrap*)_this)->do_workthread_loop();
    return NULL;
}

void kcp_client_wrap::do_workthread_loop(void)
{
    workthread_start_ = true;
    kcp_last_update_clock_ = iclock64() - KCP_UPDATE_INTERVAL;

    while (workthread_want_stop_ == false)
    {
        uint64_t cur_clock = iclock64();
        if (cur_clock - kcp_last_update_clock_ >= KCP_UPDATE_INTERVAL || cur_clock < kcp_last_update_clock_)
        {
            kcp_last_update_clock_ = cur_clock;
            kcp_client_.update();
        }
        else
        {
            millisecond_sleep(1);
        }
    }

    return;
}

void kcp_client_wrap::stop()
{
    if (workthread_start_)
    {
        workthread_want_stop_ = true;
        while (workthread_stopped_ == false)
            millisecond_sleep(1);
        workthread_start_ = false;
    }
    kcp_client_.stop();
}

void kcp_client_wrap::send_msg(const std::string& msg)
{
    kcp_client_.send_msg(msg);
}




} // end of asio_kcp
