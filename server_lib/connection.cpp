#include "connection.hpp"
#include <algorithm>
#include <boost/bind.hpp>

#include "../essential/utility/strutil.h"
#include "../util/ikcp.h"
#include "../util/connect_packet.hpp"
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include "asio_kcp_log.hpp"
#include "connection_manager.hpp"


namespace kcp_svr {

//using namespace boost::asio::ip;

connection::connection(const std::weak_ptr<connection_manager>& manager_ptr) :
    connection_manager_weak_ptr_(manager_ptr),
    conv_(0),
    p_kcp_(NULL),
    last_packet_recv_time_(0)
{
}

connection::~connection(void)
{
    clean();
}

void connection::clean(void)
{
    std::cout << "clean connection conv:" << conv_ << std::endl;
    std::string disconnect_msg = asio_kcp::making_disconnect_packet(conv_);
    send_udp_package(disconnect_msg.c_str(), disconnect_msg.size());
    ikcp_release(p_kcp_);
    p_kcp_ = NULL;
    conv_ = 0;
}

connection::shared_ptr connection::create(const std::weak_ptr<connection_manager>& manager_ptr,
        const kcp_conv_t& conv, const udp::endpoint& udp_remote_endpoint)
{
    shared_ptr ptr = std::make_shared<connection>(manager_ptr);
    if (ptr)
    {
        ptr->init_kcp(conv);
        ptr->set_udp_remote_endpoint(udp_remote_endpoint);
        AK_INFO_LOG << "new connection from: " << udp_remote_endpoint;
    }
    return ptr;
}

void connection::set_udp_remote_endpoint(const udp::endpoint& udp_remote_endpoint)
{
    udp_remote_endpoint_ = udp_remote_endpoint;
}

void connection::init_kcp(const kcp_conv_t& conv)
{
    conv_ = conv;
    p_kcp_ = ikcp_create(conv, (void*)this);
    p_kcp_->output = &connection::udp_output;

    // 启动快速模式
    // 第二个参数 nodelay-启用以后若干常规加速将启动
    // 第三个参数 interval为内部处理时钟，默认设置为 10ms
    // 第四个参数 resend为快速重传指标，设置为2
    // 第五个参数 为是否禁用常规流控，这里禁止
    //ikcp_nodelay(p_kcp_, 1, 10, 2, 1);
    ikcp_nodelay(p_kcp_, 1, 5, 1, 1); // 设置成1次ACK跨越直接重传, 这样反应速度会更快. 内部时钟5毫秒.
}

// 发送一个 udp包
int connection::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    ((connection*)user)->send_udp_package(buf, len);
	return 0;
}

void connection::send_udp_package(const char *buf, int len)
{
    if (auto ptr = connection_manager_weak_ptr_.lock())
    {
        ptr->send_udp_packet(std::string(buf, len), udp_remote_endpoint_);

    #if AK_ENABLE_UDP_PACKET_LOG
        std::cout << "udp_send:" << udp_remote_endpoint_.address().to_string() << ":" << udp_remote_endpoint_.port()
            << " conv:" << conv_
            << " size:" << len << "\n"
            << Essential::ToHexDumpText(std::string(buf, len), 32);
    #endif
    }
}

void connection::send_kcp_msg(const std::string& msg)
{
    int send_ret = ikcp_send(p_kcp_, msg.c_str(), msg.size());
    if (send_ret < 0)
    {
        std::cout << "send_ret<0: " << send_ret << std::endl;
    }
}

void connection::input(char* udp_data, size_t bytes_recvd, const udp::endpoint& udp_remote_endpoint)
{
    last_packet_recv_time_ = get_cur_clock();
    udp_remote_endpoint_ = udp_remote_endpoint;

    ikcp_input(p_kcp_, udp_data, bytes_recvd);

    /* todo need re recv or not?
    while (true)
    {
        char kcp_buf[1024 * 1000] = "";
        int kcp_recvd_bytes = ikcp_recv(p_kcp_, kcp_buf, sizeof(kcp_buf));
        if (kcp_recvd_bytes <= 0)
        {
            std::cout << "\nkcp_recvd_bytes<=0: " << kcp_recvd_bytes << std::endl;
            break;
        }
        const std::string package(kcp_buf, kcp_recvd_bytes);
        std::cout << "\nkcp recv: " << kcp_recvd_bytes << std::endl << Essential::ToHexDumpText(package, 32) << std::endl;

        ikcp_input(p_kcp_, "", 0);
    }
    */

    {
        char kcp_buf[1024 * 1000] = "";
        int kcp_recvd_bytes = ikcp_recv(p_kcp_, kcp_buf, sizeof(kcp_buf));
        if (kcp_recvd_bytes <= 0)
        {
            std::cout << "\nkcp_recvd_bytes<=0: " << kcp_recvd_bytes << std::endl;
        }
        else
        {
            const std::string package(kcp_buf, kcp_recvd_bytes);
            std::cout << "\n" << last_packet_recv_time_
                << " conv:" << conv_
                << " lag_time:" << get_cur_clock() - last_packet_recv_time_
                << " kcp recv: " << kcp_recvd_bytes << std::endl <<
                Essential::ToHexDumpText(package, 32) << std::endl;
            if (auto ptr = connection_manager_weak_ptr_.lock())
            {
                ptr->call_event_callback_func(conv_, eRcvMsg, std::make_shared<std::string>(package));
            }
        }
    }
}

void connection::update_kcp(uint32_t clock)
{
    ikcp_update(p_kcp_, clock);
}


bool connection::is_timeout(void) const
{
    if (last_packet_recv_time_ == 0)
        return false;

    return get_cur_clock() - last_packet_recv_time_ > get_timeout_time();
}

void connection::do_timeout(void)
{
    if (auto ptr = connection_manager_weak_ptr_.lock())
    {
        std::shared_ptr<std::string> msg(new std::string("timeout"));
        ptr->call_event_callback_func(conv_, eEventType::eDisconnect, msg);
    }
}


uint32_t connection::get_timeout_time(void) const
{
    return ASIO_KCP_CONNECTION_TIMEOUT_TIME;
}

uint32_t connection::get_cur_clock(void) const
{
    if (auto ptr = connection_manager_weak_ptr_.lock())
    {
        return ptr->get_cur_clock();
    }
    return 0;
}

} // namespace kcp_svr
