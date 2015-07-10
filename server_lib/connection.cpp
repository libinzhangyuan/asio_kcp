#include "connection.hpp"
#include <algorithm>
#include <boost/bind.hpp>

#include "../essential/utility/strutil.h"
#include "../util/ikcp.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include "asio_kcp_log.hpp"


namespace kcp_svr {

//using namespace boost::asio::ip;

connection::connection(udp::socket& udp_socket) :
    udp_socket_(udp_socket),
    conv_(0),
    p_kcp_(NULL)
{
}

connection::~connection(void)
{
    ikcp_release(p_kcp_);
    p_kcp_ = NULL;
    conv_ = 0;
}

connection::shared_ptr connection::create(udp::socket& udp_socket, const kcp_conv_t& conv, const udp::endpoint& udp_sender_endpoint)
{
    shared_ptr ptr = std::make_shared<connection>(udp_socket);
    if (ptr)
    {
        ptr->init_kcp(conv);
        ptr->set_udp_sender_endpoint(udp_sender_endpoint);
        AK_LOG(INFO) << "new connection from: " << udp_sender_endpoint;
    }
    return ptr;
}

void connection::set_udp_sender_endpoint(const udp::endpoint& udp_sender_endpoint)
{
    udp_sender_endpoint_ = udp_sender_endpoint;
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
    udp_socket_.send_to(boost::asio::buffer(buf, len), udp_sender_endpoint_);
    //std::cout << "\nudp send: " << len << std::endl << Essential::ToHexDumpText(std::string(buf, len), 32) << std::endl;
}

void connection::send_kcp_msg(const std::string& msg)
{
    int send_ret = ikcp_send(p_kcp_, msg.c_str(), msg.size());
    if (send_ret < 0)
    {
        std::cout << "send_ret<0: " << send_ret << std::endl;
    }
}

void connection::send_back_udp_package_by_kcp(const std::string& package)
{
    send_kcp_msg(package);
}

void connection::input(char* udp_data, size_t bytes_recvd, const udp::endpoint& udp_sender_endpoint)
{
    udp_sender_endpoint_ = udp_sender_endpoint;

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
        send_back_udp_package_by_kcp(package);

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
            std::cout << "\nkcp recv: " << kcp_recvd_bytes << std::endl << Essential::ToHexDumpText(package, 32) << std::endl;
            send_back_udp_package_by_kcp(package);
        }
    }
}

void connection::update_kcp(uint32_t clock)
{
    ikcp_update(p_kcp_, clock);
}

} // namespace kcp_svr
