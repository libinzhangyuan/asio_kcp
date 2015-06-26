//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"
#include <algorithm>
#include <boost/bind.hpp>

#include "../essential/utility/strutil.h"
#include "../net/ikcp.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
//#include <sys/wait.h>
//#include <sys/types.h>

/* get system time */
static inline void itimeofday(long *sec, long *usec)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
}

/* get clock in millisecond 64 */
static inline uint64_t iclock64(void)
{
    long s, u;
    uint64_t value;
    itimeofday(&s, &u);
    value = ((uint64_t)s) * 1000 + (u / 1000);
    return value;
}


static inline IUINT32 iclock()
{
    return (IUINT32)(iclock64() & 0xfffffffful);
}


namespace server {
using namespace boost::asio::ip;


static connection_manager* static_p_connection_manager = NULL;

connection_manager::connection_manager(boost::asio::io_service& io_service, const std::string& address, int udp_port) :
    stopped_(false),
    udp_socket_(io_service, udp::endpoint(boost::asio::ip::address_v4::from_string(address), udp_port)),
    kcp_timer_(io_service),
    p_kcp_(NULL)
{
    static_p_connection_manager = this;
    //udp_socket_.set_option(boost::asio::ip::udp::socket::non_blocking_io(true));

    init_kcp();
    hook_udp_async_receive();
    hook_kcp_timer();
}

void connection_manager::stop_all()
{
  udp_socket_.cancel();
  udp_socket_.close();
  stopped_ = true;
}

void connection_manager::send_kcp_msg(const std::string& msg)
{
    int send_ret = ikcp_send(p_kcp_, msg.c_str(), msg.size());
    if (send_ret < 0)
    {
        std::cout << "send_ret<0: " << send_ret << std::endl;
    }
}


void connection_manager::check_udp_package(size_t bytes_recvd)
{
    return;
    static size_t count_sum = 0;
    static size_t count_error_packet = 0;

    count_sum++;
    if ((count_sum % 1000) == 0)
        printf("sum: %ld  ", count_sum);
    if ((count_sum % 10000) == 0)
        printf("   error_count: %ld\n", count_error_packet);

    if (bytes_recvd != 5824)
    {
        count_error_packet += 1;
        printf("\ncount error\nrecv_udp: %ld count\n%s\n", bytes_recvd,
                Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32).c_str());
        return;
    }

    for (size_t i = 0; i < 91; ++i)
    {
        char a = '!' + i;
        for (size_t j = i * 64; j < (i + 1) * 64; ++j)
        {
            if (udp_data_[j] != a)
            {
                count_error_packet += 1;
                printf("\nerror at %c\nrecv_udp: %ld count\n%s\n", a, bytes_recvd,
                        Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32).c_str());
                return;
            }
        }
    }
}

void connection_manager::send_back_udp_package_by_kcp(const std::string& package)
{
    std::cout << "send_back_udp_package_by_kcp" << std::endl;
    send_kcp_msg(package);
}

void connection_manager::handle_udp_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
{
    if (!error && bytes_recvd > 0)
    {
        std::cout << "\nudp_sender_endpoint: " << udp_sender_endpoint_ << std::endl;
        unsigned long addr_i = udp_sender_endpoint_.address().to_v4().to_ulong();
        std::cout << addr_i << " " << udp_sender_endpoint_.port() << std::endl;
        std::cout << "udp recv: " << bytes_recvd << std::endl <<
            Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32) << std::endl;
        check_udp_package(bytes_recvd);

        const std::string& package = recv_udp_package_from_kcp(bytes_recvd);
        if (package.size() > 0)
            send_back_udp_package_by_kcp(package);
        hook_udp_async_receive();
        return;
    }

    printf("\nhandle_udp_receive_from error end! error: %s, bytes_recvd: %ld\n", error.message().c_str(), bytes_recvd);
}

void connection_manager::hook_udp_async_receive(void)
{
    if (stopped_)
        return;
    udp_socket_.async_receive_from(
          boost::asio::buffer(udp_data_, sizeof(udp_data_)), udp_sender_endpoint_,
          boost::bind(&connection_manager::handle_udp_receive_from, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
}

void connection_manager::init_kcp(void)
{
    p_kcp_ = ikcp_create(123456, (void*)10);
    p_kcp_->output = &connection_manager::udp_output;

    // 启动快速模式
    // 第二个参数 nodelay-启用以后若干常规加速将启动
    // 第三个参数 interval为内部处理时钟，默认设置为 10ms
    // 第四个参数 resend为快速重传指标，设置为2
    // 第五个参数 为是否禁用常规流控，这里禁止
    ikcp_nodelay(p_kcp_, 1, 10, 2, 1);
}

uint64_t connection_manager::endpoint_to_i(const boost::asio::ip::udp::endpoint& ep)
{
    uint64_t addr_i = ep.address().to_v4().to_ulong();
    uint32_t port = ep.port();
    return (addr_i << 32) + port;
}

// 发送一个 udp包
int connection_manager::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    static_p_connection_manager->send_udp_package(buf, len);
	return 0;
}

void connection_manager::send_udp_package(const char *buf, int len)
{
    udp_socket_.send_to(boost::asio::buffer(buf, len), udp_sender_endpoint_);
    std::cout << "\nudp send: " << len << std::endl << Essential::ToHexDumpText(std::string(buf, len), 32) << std::endl;
}


std::string connection_manager::recv_udp_package_from_kcp(size_t bytes_recvd)
{
    ikcp_input(p_kcp_, udp_data_, bytes_recvd);
    char kcp_buf[1024 * 1000] = "";
    int kcp_recvd_bytes = ikcp_recv(p_kcp_, kcp_buf, sizeof(kcp_buf));
    if (kcp_recvd_bytes < 0)
    {
        std::cout << "\nkcp_recvd_bytes<0: " << kcp_recvd_bytes << std::endl;
        return "";
    }
    const std::string result(kcp_buf, kcp_recvd_bytes);
    std::cout << "\nkcp recv: " << kcp_recvd_bytes << std::endl << Essential::ToHexDumpText(result, 32) << std::endl;
    return result;
}

void connection_manager::hook_kcp_timer(void)
{
    if (stopped_)
        return;
    kcp_timer_.expires_from_now(boost::posix_time::milliseconds(5));
    kcp_timer_.async_wait(std::bind(&connection_manager::handle_kcp_time, this));
}

void connection_manager::handle_kcp_time(void)
{
    hook_kcp_timer();
    ikcp_update(p_kcp_, iclock());
}


} // namespace server
