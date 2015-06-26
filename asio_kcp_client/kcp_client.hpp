//
// kcp_client.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef _BS_kcp_client_HPP
#define _BS_kcp_client_HPP

#include <set>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

struct IKCPCB;
typedef struct IKCPCB ikcpcb;

namespace server {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class kcp_client
  : private boost::noncopyable
{
public:
    kcp_client(boost::asio::io_service& io_service, int udp_port_bind, const std::string& server_ip, const int server_port);

    // user level send msg.
    void send_msg(const std::string& msg);

    /// Stop all connections.
    void stop_all();

private:
    void init_kcp(void);
    void init_input(void);

private:
    bool stopped_;

    // low level function for send udp packet.
    void send_udp_package(const char *buf, int len);

    void handle_udp_receive_from(const boost::system::error_code& error, size_t bytes_recvd);
    void hook_udp_async_receive(void);
    void check_udp_package(size_t bytes_recvd);
    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);
    static uint64_t endpoint_to_i(const boost::asio::ip::udp::endpoint& ep);
    void recv_udp_package_from_kcp(size_t bytes_recvd);
    void handle_kcp_time(void);
    void hook_kcp_timer(void);

    void hook_input_handle(void);
    void input_handler(const boost::system::error_code& error, const size_t bytes_transferred);

    /// The UDP
    boost::asio::ip::udp::socket udp_socket_;
    boost::asio::ip::udp::endpoint dst_end_point_;
    //enum { udp_packet_max_length = 548 }; // maybe 1472 will be ok.
    enum { udp_packet_max_length = 1080 }; // (576-8-20 - 8) * 2
    char udp_data_[1024 * 32];

    boost::asio::deadline_timer kcp_timer_;
    ikcpcb* p_kcp_; // --own

    boost::asio::posix::stream_descriptor input_;
    boost::asio::streambuf input_buf_;
};

} // namespace server

#endif // _BS_kcp_client_HPP
