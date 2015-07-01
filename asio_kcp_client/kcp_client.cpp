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
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../essential/utility/strutil.h"
#include "../net/ikcp.h"
#include "test_util.h"

#define PACKAGE_LOSE_RATIO 0
#define PACKAGE_CONTENT_DAMAGE_RATIO 0
#define SEND_TEST_MSG_INTERVAL 50

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

#define CLOCK_INTERVAL_STR "_"
std::string make_test_str(size_t test_str_size)
{
    std::ostringstream ostr;
    ostr << iclock64();
    std::string msg_str = ostr.str();
    msg_str += test_str(CLOCK_INTERVAL_STR, test_str_size - msg_str.size());
    return msg_str;
}

std::string get_cur_time_str()
{
    time_t tmpcal_ptr = {0};
    struct tm *tmp_ptr = NULL;
    tmpcal_ptr = time(NULL);
    tmp_ptr = localtime(&tmpcal_ptr);
    std::ostringstream osstrm;
    osstrm << tmp_ptr->tm_hour << ":" << tmp_ptr->tm_min << "." << tmp_ptr->tm_sec;
    return osstrm.str();
}

uint64_t get_time_from_msg(const std::string& msg)
{
    std::size_t pos = msg.find(CLOCK_INTERVAL_STR);
    if (pos == std::string::npos)
    {
        std::cout << "wrong msg: " << msg << std::endl;
        return 0;
    }
    const std::string& time_str = msg.substr(0, pos);
    return std::atoll(time_str.c_str());
}

namespace server {
using namespace boost::asio::ip;


static kcp_client* static_p_kcp_client = NULL;

kcp_client::kcp_client(boost::asio::io_service& io_service, int udp_port_bind,
        const std::string& server_ip, const int server_port, size_t test_str_size) :
    stopped_(false),
    udp_socket_(io_service, udp::endpoint(udp::v4(), udp_port_bind)),
    dst_end_point_(boost::asio::ip::address::from_string(server_ip), server_port),
    kcp_timer_(io_service),
    kcp_timer_send_msg_(io_service),
    p_kcp_(NULL),
    input_(io_service),
    test_str_size_(test_str_size)
{
    static_p_kcp_client = this;

    init_kcp();
    hook_udp_async_receive();
    hook_kcp_timer();
    hook_timer_send_msg();

    send_test_msg();
}

void kcp_client::stop_all()
{
  udp_socket_.cancel();
  udp_socket_.close();
  stopped_ = true;
}

void kcp_client::send_test_msg(void)
{
    send_msg(make_test_str(test_str_size_));
}

void kcp_client::send_msg(const std::string& msg)
{
    int send_ret = ikcp_send(p_kcp_, msg.c_str(), msg.size());
    if (send_ret < 0)
    {
        std::cout << "send_ret<0: " << send_ret << std::endl;
    }
}

void kcp_client::print_recv_log(const std::string& msg)
{
    static size_t static_good_recv_count = 0;
    static uint64_t static_last_refresh_time = 0;
    static size_t static_recved_bytes = 0;
    static_recved_bytes += msg.size();
    uint64_t cur_time = iclock64();
    uint64_t send_time = get_time_from_msg(msg);
    uint64_t interval = cur_time - send_time;

    static_good_recv_count++;
    recv_package_interval_.push_back(interval);
    recv_package_interval10_.push_back(interval);

    std::cout << interval << "\t";

    if (static_good_recv_count % 10 == 0)
    {

        int average10 = 0;
        for (int x : recv_package_interval10_)
            average10 += x;
        average10 = (average10 / 10);
        recv_package_interval10_.clear();

        int average_total = 0;
        for (int x: recv_package_interval_)
            average_total += x;
        average_total = average_total / recv_package_interval_.size();

        std::cout << "max: " << *std::max_element( recv_package_interval_.begin(), recv_package_interval_.end() ) <<
            "  average 10: " << average10 <<
            "  average total: " << average_total;
        if (cur_time - static_last_refresh_time > 10 * 1000)
        {
            std::cout << " " << static_cast<double>(static_recved_bytes * 10 / (cur_time - static_last_refresh_time)) / 10 << "KB/s(in)";
            static_last_refresh_time = cur_time;
            static_recved_bytes = 0;
        }
        std::cout << std::endl;
        std::cout << get_cur_time_str() << " ";
        recv_package_interval_.clear();
    }
    std::cout.flush();
}

void kcp_client::handle_udp_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
{
    if (!error && bytes_recvd > 0)
    {
        //std::cout << "\nudp_sender_endpoint: " << dst_end_point_ << std::endl;
        //unsigned long addr_i = dst_end_point_.address().to_v4().to_ulong();
        //std::cout << addr_i << " " << dst_end_point_.port() << std::endl;
        //std::cout << "udp recv: " << bytes_recvd << std::endl <<
        //    Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32) << std::endl;

        // 丢包测试
        if (PACKAGE_LOSE_RATIO > 0)
        {
            if (std::rand() % 100 < PACKAGE_LOSE_RATIO)
            {
                std::cout << "udp recv lose package" << std::endl;
                hook_udp_async_receive();
                return; // 丢包
            }
        }

        ikcp_input(p_kcp_, udp_data_, bytes_recvd);


        while (true)
        {
            const std::string& msg = recv_udp_package_from_kcp();
            //std::cout << "recv kcp msg: " << msg << std::endl;
            if (msg.size() > 0)
            {
                if (msg.size() != test_str_size_)
                {
                    std::cout << "recv wrong msg" << std::endl;
                    break;
                }
                else
                {
                    // recved good msg.

                    print_recv_log(msg);
                    continue;
                }
            }
            break;
        }

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
    //std::cout << "send_udp_package: " << static_send_udp_packet_count << "   - " << get_milly_sec_time_str() << std::endl;


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
    //std::cout << "udp send: " << len << std::endl << Essential::ToHexDumpText(std::string(buf, len), 32) << std::endl;
}

std::string kcp_client::recv_udp_package_from_kcp()
{
    char kcp_buf[1024 * 1000] = "";
    int kcp_recvd_bytes = ikcp_recv(p_kcp_, kcp_buf, sizeof(kcp_buf));
    if (kcp_recvd_bytes < 0)
    {
        //std::cout << "kcp_recvd_bytes<0: " << kcp_recvd_bytes << std::endl;
        return "";
    }

    const std::string result(kcp_buf, kcp_recvd_bytes);
    //std::cout << "kcp recv: " << kcp_recvd_bytes << std::endl << Essential::ToHexDumpText(result, 32) << std::endl;
    return result;
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

void kcp_client::hook_timer_send_msg(void)
{
    if (stopped_)
        return;
    kcp_timer_send_msg_.expires_from_now(boost::posix_time::milliseconds(SEND_TEST_MSG_INTERVAL));
    kcp_timer_send_msg_.async_wait(std::bind(&kcp_client::handle_timer_send_msg, this));
}

void kcp_client::handle_timer_send_msg(void)
{
    send_test_msg();
    hook_timer_send_msg();
}

} // namespace server
