//
// connection_manager.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef _BS_CONNECTION_MANAGER_HPP
#define _BS_CONNECTION_MANAGER_HPP

#include <set>
#include <unordered_map>

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

struct IKCPCB;
typedef struct IKCPCB ikcpcb;

namespace server {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class connection_manager
  : private boost::noncopyable
{
public:
    connection_manager(boost::asio::io_service& io_service, const std::string& address, int udp_port);

    /// Stop all connections.
    void stop_all();


    // user level send msg.
    void send_kcp_msg(const std::string& msg);

private:
    void init_kcp(void);
    void send_udp_package(const char *buf, int len);

private:
    bool stopped_;

    /// The UDP
    void handle_udp_receive_from(const boost::system::error_code& error, size_t bytes_recvd);
    void hook_udp_async_receive(void);
    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);
    static uint64_t endpoint_to_i(const boost::asio::ip::udp::endpoint& ep);
    std::string recv_udp_package_from_kcp(size_t bytes_recvd);
    void send_back_udp_package_by_kcp(const std::string& package);
    void handle_kcp_time(void);
    void hook_kcp_timer(void);

    /// The UDP
    boost::asio::ip::udp::socket udp_socket_;
    boost::asio::ip::udp::endpoint udp_sender_endpoint_;
    //enum { udp_packet_max_length = 548 }; // maybe 1472 will be ok.
    enum { udp_packet_max_length = 1080 }; // (576-8-20 - 8) * 2
    char udp_data_[1024 * 32];

    boost::asio::deadline_timer kcp_timer_;
    ikcpcb* p_kcp_; // --own
};

} // namespace server

#endif // _BS_CONNECTION_MANAGER_HPP
