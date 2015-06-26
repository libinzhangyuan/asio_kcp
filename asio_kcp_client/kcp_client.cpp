//
// kcp_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "kcp_client.hpp"
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
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define PACKAGE_LOSE_RATIO 66
#define PACKAGE_CONTENT_DAMAGE_RATIO 0

/* get system time */
static inline void itimeofday(long *sec, long *usec)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
}

/* get clock in millisecond 64 */
static inline IINT64 iclock64(void)
{
    long s, u;
    IINT64 value;
    itimeofday(&s, &u);
    value = ((IINT64)s) * 1000 + (u / 1000);
    return value;
}


static inline IUINT32 iclock()
{
    return (IUINT32)(iclock64() & 0xfffffffful);
}

std::string get_milly_sec_time_str(void)
{
    boost::posix_time::ptime ptime = boost::posix_time::microsec_clock::universal_time();
    return boost::posix_time::to_iso_extended_string(ptime);
}


namespace server {
using namespace boost::asio::ip;


static kcp_client* static_p_kcp_client = NULL;

kcp_client::kcp_client(boost::asio::io_service& io_service, int udp_port_bind,
        const std::string& server_ip, const int server_port) :
    stopped_(false),
    udp_socket_(io_service, udp::endpoint(udp::v4(), udp_port_bind)),
    dst_end_point_(boost::asio::ip::address::from_string(server_ip), server_port),
    kcp_timer_(io_service),
    p_kcp_(NULL),
    input_(io_service)
{
    static_p_kcp_client = this;

    init_input();

    init_kcp();
    hook_udp_async_receive();
    hook_kcp_timer();
}

void kcp_client::stop_all()
{
  udp_socket_.cancel();
  udp_socket_.close();
  stopped_ = true;
}

void kcp_client::init_input(void)
{
    input_.assign( STDIN_FILENO );
    hook_input_handle();
}

void kcp_client::hook_input_handle(void)
{
    static char delim('\n');
    // reserve 512 bytes in output sequence
    // boost::asio::streambuf::mutable_buffers_type bufs = input_buf_.prepare(512);

    boost::asio::async_read_until(
            input_,
            input_buf_,
            delim,
            boost::bind(
                &kcp_client::input_handler,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void kcp_client::input_handler(const boost::system::error_code& error, const size_t bytes_transferred)
{
    if ( error ) {
        std::cerr << "read error: " << boost::system::system_error(error).what() << std::endl;
        return;
    }

    if (bytes_transferred > 0)
    {
        std::istream is(&input_buf_);
        std::string input_str;
        is >> input_str;

        std::cout << "You input: input_str.size=" << input_str.size() << "  bytes_transferred=" << bytes_transferred << std::endl <<
            Essential::ToHexDumpText(input_str, 32) << std::endl;
        if ( input_str != "\n" && input_str != "")
            send_msg(input_str);
    }

    hook_input_handle();
}

void kcp_client::check_udp_package(size_t bytes_recvd)
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
        printf("\ncount error\nrecv_udp: %ld count\n%s\n", bytes_recvd, Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32).c_str());
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
                printf("\nerror at %c\nrecv_udp: %ld count\n%s\n", a, bytes_recvd, Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32).c_str());
                return;
            }
        }
    }
}

void kcp_client::send_msg(const std::string& msg)
{
    int send_ret = ikcp_send(p_kcp_, msg.c_str(), msg.size());
    if (send_ret < 0)
    {
        std::cout << "send_ret<0: " << send_ret << std::endl;
    }
}

void kcp_client::handle_udp_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
{
    if (!error && bytes_recvd > 0)
    {
        std::cout << "\nudp_sender_endpoint: " << dst_end_point_ << std::endl;
        unsigned long addr_i = dst_end_point_.address().to_v4().to_ulong();
        std::cout << addr_i << " " << dst_end_point_.port() << std::endl;
        std::cout << "udp recv: " << bytes_recvd << std::endl <<
            Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32) << std::endl;
        check_udp_package(bytes_recvd);

        recv_udp_package_from_kcp(bytes_recvd);
        //send_back_udp_package_by_kcp();

        hook_udp_async_receive();
        return;
    }

    printf("\nhandle_udp_receive_from error end! error: %s, bytes_recvd: %ld\n", error.message().c_str(), bytes_recvd);
}

void kcp_client::hook_udp_async_receive(void)
{
    if (stopped_)
        return;
    udp_socket_.async_receive_from(
          boost::asio::buffer(udp_data_, sizeof(udp_data_)), dst_end_point_,
          boost::bind(&kcp_client::handle_udp_receive_from, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
}

void kcp_client::init_kcp(void)
{
    p_kcp_ = ikcp_create(123456, (void*)11);
    p_kcp_->output = &kcp_client::udp_output;

    // 启动快速模式
    // 第二个参数 nodelay-启用以后若干常规加速将启动
    // 第三个参数 interval为内部处理时钟，默认设置为 10ms
    // 第四个参数 resend为快速重传指标，设置为2
    // 第五个参数 为是否禁用常规流控，这里禁止
    ikcp_nodelay(p_kcp_, 1, 10, 2, 1);
}

uint64_t kcp_client::endpoint_to_i(const boost::asio::ip::udp::endpoint& ep)
{
    uint64_t addr_i = ep.address().to_v4().to_ulong();
    uint32_t port = ep.port();
    return (addr_i << 32) + port;
}

// 发送一个 udp包
int kcp_client::udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    static_p_kcp_client->send_udp_package(buf, len);
	return 0;
}

void kcp_client::send_udp_package(const char *buf, int len)
{
    static size_t static_send_udp_packet_count = 0;
    static_send_udp_packet_count++;
    std::cout << "send_udp_package: " << static_send_udp_packet_count << "   - " << get_milly_sec_time_str() << std::endl;


    // 丢包测试
    if (PACKAGE_LOSE_RATIO > 0)
    {
        if (std::rand() % 100 < PACKAGE_LOSE_RATIO)
        {
            std::cout << "udp send lose package" << std::endl;
            return; // 丢包
        }
    }

    udp_socket_.send_to(boost::asio::buffer(buf, len), dst_end_point_);
    std::cout << "udp send: " << len << std::endl << Essential::ToHexDumpText(std::string(buf, len), 32) << std::endl;
}

void kcp_client::recv_udp_package_from_kcp(size_t bytes_recvd)
{
    // 丢包测试
    if (PACKAGE_LOSE_RATIO > 0)
    {
        if (std::rand() % 100 < PACKAGE_LOSE_RATIO)
        {
            std::cout << "udp recv lose package" << std::endl;
            return; // 丢包
        }
    }


    ikcp_input(p_kcp_, udp_data_, bytes_recvd);
    char kcp_buf[1024 * 1000] = "";
    int kcp_recvd_bytes = ikcp_recv(p_kcp_, kcp_buf, sizeof(kcp_buf));
    if (kcp_recvd_bytes < 0)
    {
        std::cout << "kcp_recvd_bytes<0: " << kcp_recvd_bytes << std::endl;
        return;
    }

    const std::string result(kcp_buf, kcp_recvd_bytes);
    std::cout << "kcp recv: " << kcp_recvd_bytes << std::endl << Essential::ToHexDumpText(result, 32) << std::endl;
    return;
}

void kcp_client::hook_kcp_timer(void)
{
    if (stopped_)
        return;
    kcp_timer_.expires_from_now(boost::posix_time::milliseconds(5));
    kcp_timer_.async_wait(std::bind(&kcp_client::handle_kcp_time, this));
}

void kcp_client::handle_kcp_time(void)
{
    hook_kcp_timer();
    ikcp_update(p_kcp_, iclock());
}


} // namespace server
