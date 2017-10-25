#include "kcp_client.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sstream>
#include <fcntl.h>
#include <string.h>

#include "../util/ikcp.h"
#include "../util/connect_packet.hpp"
#include "kcp_client_util.h"

namespace asio_kcp {

kcp_client::kcp_client(void) :
    in_connect_stage_(false),
    connect_start_time_(0),
    last_send_connect_msg_time_(0),
    connect_succeed_(false),
    pevent_func_(NULL),
    event_callback_var_(NULL),
    udp_port_bind_(0),
    server_port_(0),
    udp_socket_(-1),
    p_kcp_(NULL)
{
    bzero(&servaddr_, sizeof(servaddr_));
}

kcp_client::~kcp_client(void)
{
    clean();
}

void kcp_client::clean(void)
{
    if (p_kcp_)
    {
        ikcp_release(p_kcp_);
        p_kcp_ = NULL;
    }
}

void kcp_client::set_event_callback(const client_event_callback_t& event_callback_func, void* var)
{
    pevent_func_ = &event_callback_func;
    event_callback_var_ = var;
}

void kcp_client::stop()
{
/*    set stopped_
    waiting;
        closesocket();
        close ikcp;
        delete ikcp;
*/
}

void kcp_client::init_kcp(kcp_conv_t conv)
{
    p_kcp_ = ikcp_create(conv, (void*)this);
    p_kcp_->output = &kcp_client::udp_output;

    // 启动快速模式
    // 第二个参数 nodelay-启用以后若干常规加速将启动
    // 第三个参数 interval为内部处理时钟，默认设置为 10ms
    // 第四个参数 resend为快速重传指标，设置为2
    // 第五个参数 为是否禁用常规流控，这里禁止
    //ikcp_nodelay(p_kcp_, 1, 10, 2, 1);
    ikcp_nodelay(p_kcp_, 1, 2, 1, 1); // 设置成1次ACK跨越直接重传, 这样反应速度会更快. 内部时钟5毫秒.
}

int kcp_client::connect_async(int udp_port_bind, const std::string& server_ip, const int server_port)
{
    if (udp_socket_ != -1)
        return KCP_ERR_ALREADY_CONNECTED;

    udp_port_bind_ = udp_port_bind;
    server_ip_ = server_ip;
    server_port_ = server_port;

    // init udp connect
    {
        int ret = init_udp_connect();
        if (ret < 0)
            return ret;
    }

    // do asio_kcp connect
    {
        in_connect_stage_ = true;
        connect_start_time_ = iclock64();
    }


    return 0;
}

void kcp_client::update()
{
    uint64_t cur_clock = iclock64();
    if (in_connect_stage_)
    {
        do_asio_kcp_connect(cur_clock);
        return;
    }

    if (connect_succeed_)
    {
        // send the msg in SendMsgQueue
        do_send_msg_in_queue();

        // recv the udp packet.
        do_recv_udp_packet_in_loop();

        // ikcp_update
        //
        ikcp_update(p_kcp_, cur_clock);
    }
}

void kcp_client::do_asio_kcp_connect(uint64_t cur_clock)
{
    if (connect_timeout(cur_clock))
    {
        (*pevent_func_)(0, eConnectFailed, KCP_CONNECT_TIMEOUT_MSG, event_callback_var_);
        in_connect_stage_ = false;
        return;
    }
    if (need_send_connect_packet(cur_clock))
        do_send_connect_packet(cur_clock);

    try_recv_connect_back_packet();
}

bool kcp_client::need_send_connect_packet(uint64_t cur_clock) const
{
    return (cur_clock - last_send_connect_msg_time_ > KCP_RESEND_CONNECT_MSG_INTERVAL);
}

bool kcp_client::connect_timeout(uint64_t cur_clock) const
{
    return (cur_clock - connect_start_time_ > KCP_CONNECT_TIMEOUT_TIME);
}

void kcp_client::do_send_connect_packet(uint64_t cur_clock)
{
    last_send_connect_msg_time_ = cur_clock;

    // send a connect cmd.
    std::string connect_msg = asio_kcp::making_connect_packet();
    std::cerr << "send connect packet" << std::endl;
    const ssize_t send_ret = send(udp_socket_, connect_msg.c_str(), connect_msg.size(), 0);
    if (send_ret < 0)
    {
        std::cerr << "do_asio_kcp_connect send error return with errno: " << errno << " " << strerror(errno) << std::endl;
    }
}

void kcp_client::try_recv_connect_back_packet(void)
{
    char recv_buf[1400] = ""; // connect udp packet will not bigger than 1400.
    const ssize_t ret_recv = recv(udp_socket_, recv_buf, sizeof(recv_buf), 0); // recv timeout is 2 milliseconds. - SO_RCVTIMEO
    if (ret_recv < 0)
    {
        int err = errno;
        if (err == EAGAIN)
            return;
        std::cerr << "try_recv_connect_back_packet recv error return with errno: " << err << " " << strerror(err) << std::endl;
    }
    if (ret_recv > 0 && asio_kcp::is_send_back_conv_packet(recv_buf, ret_recv))
    {
        // connect ok.

        kcp_conv_t conv = asio_kcp::grab_conv_from_send_back_conv_packet(recv_buf, ret_recv);

        std::cerr << "connect succeed in " << iclock64() - connect_start_time_ << " milliseconds"
            << " conv:" << conv
            << std::endl;
        init_kcp(conv);
        in_connect_stage_ = false;
        connect_succeed_ = true;
        (*pevent_func_)(p_kcp_->conv, eConnect, "connect succeed", event_callback_var_);
    }
}
/*
int kcp_client::do_asio_kcp_connect_old(void)
{
    char recv_buf[1400] = ""; // connect udp packet will not bigger than 1400.
    std::string connect_msg = asio_kcp::making_connect_packet();

    // loop 5 seconds for try.
    time_t begin_time = ::time(NULL);
    while (true)
    {
        time_t cur_time = ::time(NULL);
        if (cur_time - begin_time > 5)
            return KCP_ERR_KCP_CONNECT_FAIL;

        // send a connect cmd.
        std::cerr << "send connect packet" << std::endl;
        const ssize_t send_ret = send(udp_socket_, connect_msg.c_str(), connect_msg.size(), 0);
        if (send_ret < 0)
        {
            std::cerr << "do_asio_kcp_connect send error return with errno: " << errno << " " << strerror(errno) << std::endl;
        }

        // try recv in 500 milliseconds
        for (int j = 0; j < 250; j++)
        {
            const ssize_t ret_recv = recv(udp_socket_, recv_buf, sizeof(recv_buf), 0); // recv timeout is 2 milliseconds. - SO_RCVTIMEO
            if (ret_recv < 0)
            {
                int err = errno;
                if (err == EAGAIN)
                    continue;
                std::cerr << "do_asio_kcp_connect recv error return with errno: " << err << " " << strerror(err) << std::endl;
            }
            if (ret_recv > 0 && asio_kcp::is_send_back_conv_packet(recv_buf, ret_recv))
            {
                // connect ok.
                std::cerr << "connect succeed in " << ::time(NULL) - begin_time << " seconds" << std::endl;
                // save conv when recved connect back packet.
                kcp_conv_t conv = asio_kcp::grab_conv_from_send_back_conv_packet(recv_buf, ret_recv);
                // init p_kcp_
                init_kcp(conv);
                return 0;
            }

        }
    }
}
*/
int kcp_client::init_udp_connect(void)
{
    // servaddr_
    {
        servaddr_.sin_family = AF_INET;
        servaddr_.sin_port = htons(server_port_);
        int ret = inet_pton(AF_INET, server_ip_.c_str(), &servaddr_.sin_addr);
        if (ret <= 0)
        {
            if (ret < 0) // errno set
                std::cerr << "inet_pton error return < 0, with errno: " << errno << " " << strerror(errno) << std::endl;
            else
                std::cerr << "inet_pton error return 0" << std::endl;
            return KCP_ERR_ADDRESS_INVALID;
        }
    }

    // create udp_socket_
    {
        udp_socket_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_socket_ < 0)
        {
            std::cerr << "socket error return with errno: " << errno << " " << strerror(errno) << std::endl;
            return KCP_ERR_CREATE_SOCKET_FAIL;
        }
    }

    // set socket recv timeout
    /*{
        struct timeval recv_timeo;
        recv_timeo.tv_sec = 0;
        recv_timeo.tv_usec = 2 * 1000; // 2 milliseconds
        int ret = setsockopt(udp_socket_, SOL_SOCKET, SO_RCVTIMEO, &recv_timeo, sizeof(recv_timeo));
        if (ret < 0)
        {
            std::cerr << "setsockopt error return with errno: " << errno << " " << strerror(errno) << std::endl;
        }
    }*/

    // set socket non-blocking
    {
        int flags = fcntl(udp_socket_, F_GETFL, 0);
        if (flags == -1)
        {
            std::cerr << "get socket non-blocking: fcntl error return with errno: " << errno << " " << strerror(errno) << std::endl;
            return KCP_ERR_SET_NON_BLOCK_FAIL;
        }
        int ret = fcntl(udp_socket_, F_SETFL, flags | O_NONBLOCK);
        if (ret == -1)
        {
            std::cerr << "set socket non-blocking: fcntl error return with errno: " << errno << " " << strerror(errno) << std::endl;
            return KCP_ERR_SET_NON_BLOCK_FAIL;
        }
    }

    // set recv buf bigger

    // bind
    if (udp_port_bind_ != 0)
    {
        struct sockaddr_in bind_addr;
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_port = htons(udp_port_bind_);
        bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        int ret_bind = ::bind(udp_socket_, (const struct sockaddr*)(&bind_addr), sizeof(bind_addr));
        if (ret_bind < 0)
            std::cerr << "setsockopt error return with errno: " << errno << " " << strerror(errno) << std::endl;
    }

    // udp connect
    {
        int ret = ::connect(udp_socket_, (const struct sockaddr*)(&servaddr_), sizeof(servaddr_));
        if (ret < 0)
        {
            std::cerr << "connect error return with errno: " << errno << " " << strerror(errno) << std::endl;
            return KCP_ERR_CONNECT_FUNC_FAIL;
        }
    }

    return 0;
}

void kcp_client::do_recv_udp_packet_in_loop(void)
{
    char recv_buf[MAX_MSG_SIZE * 2] = ""; // udp packet will not twice bigger than kcp msg size.
    const ssize_t ret_recv = recv(udp_socket_, recv_buf, sizeof(recv_buf), 0);
    if (ret_recv < 0)
    {
        int err = errno;
        if (err == EAGAIN)
            return;
        std::ostringstream ostrm;
        std::string err_detail = ostrm.str();
        ostrm << "do_asio_kcp_connect recv error return with errno: " << err << " " << strerror(err);
        std::cerr << err_detail << std::endl;
        (*pevent_func_)(p_kcp_->conv, eDisconnect, err_detail, event_callback_var_);
        return;
    }

    if (ret_recv == 0)
        return; // do nothing.   ignore the zero size packet.

    // ret_recv > 0
    handle_udp_packet(std::string(recv_buf, ret_recv));
    return;
}

int kcp_client::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    ((kcp_client*)user)->send_udp_package(buf, len);
	return 0;
}

void kcp_client::send_udp_package(const char *buf, int len)
{
    std::cerr << "send_udp_package" << std::endl;
    const ssize_t send_ret = send(udp_socket_, buf, len, 0);
    if (send_ret < 0)
    {
        std::cerr << "send_udp_package error with errno: " << errno << " " << strerror(errno) << std::endl;
    }
    else if (send_ret != len)
    {
        std::cerr << "send_udp_package error: not all packet send. " << send_ret << " in " << len << std::endl;
    }
}

void kcp_client::send_msg(const std::string& msg)
{
    // todo: check msg size < MAX_MSG_SIZE

    send_msg_queue_.push(msg);
}

void kcp_client::do_send_msg_in_queue(void)
{
    std::queue<std::string> msgs = send_msg_queue_.grab_all();

    while (msgs.size() > 0)
    {
        std::string msg = msgs.front();
        int send_ret = ikcp_send(p_kcp_, msg.c_str(), msg.size());
        if (send_ret < 0)
        {
            std::cerr << "send_ret<0: " << send_ret << std::endl;
        }
        msgs.pop();
    }
}

void kcp_client::handle_udp_packet(const std::string& udp_packet)
{
    if (is_disconnect_packet(udp_packet.c_str(), udp_packet.size()))
    {
        if (pevent_func_ != NULL)
        {
            std::string msg(udp_packet);
            (*pevent_func_)(p_kcp_->conv, eDisconnect, msg, event_callback_var_);
        }
        return;
    }


    ikcp_input(p_kcp_, udp_packet.c_str(), udp_packet.size());

    while (true)
    {
        const std::string& msg = recv_udp_package_from_kcp();
        if (msg.size() > 0)
        {
            // recved good msg.
            std::cerr << "recv good kcp msg: " << msg << std::endl;
            if (pevent_func_ != NULL)
            {
                (*pevent_func_)(p_kcp_->conv, eRcvMsg, msg, event_callback_var_);
            }
            continue;
        }
        break;
    }
}

std::string kcp_client::recv_udp_package_from_kcp(void)
{
    char kcp_buf[MAX_MSG_SIZE] = "";
    int kcp_recvd_bytes = ikcp_recv(p_kcp_, kcp_buf, sizeof(kcp_buf));
    if (kcp_recvd_bytes < 0)
    {
        //std::cerr << "kcp_recvd_bytes<0: " << kcp_recvd_bytes << std::endl;
        return "";
    }

    const std::string result(kcp_buf, kcp_recvd_bytes);
    return result;
}


} // namespace asio_kcp
