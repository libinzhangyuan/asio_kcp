#ifndef _KCP_CONNECTION_MANAGER_HPP_
#define _KCP_CONNECTION_MANAGER_HPP_

#include <set>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include "connection_container.hpp"



namespace kcp_svr {

class connection_manager
  : private boost::noncopyable
{
public:
    connection_manager(boost::asio::io_service& io_service, const std::string& address, int udp_port);

    /// Stop all connections.
    void stop_all();

private:

    /// The UDP
    void handle_udp_receive_from(const boost::system::error_code& error, size_t bytes_recvd);
    void hook_udp_async_receive(void);
    static uint64_t endpoint_to_i(const udp::endpoint& ep);
    void handle_kcp_time(void);
    void hook_kcp_timer(void);

    void handle_connect_packet();
    void handle_kcp_packet(size_t bytes_recvd);

private:
    bool stopped_;

    /// The listen socket.
    udp::socket udp_socket_;

    udp::endpoint udp_sender_endpoint_;

    //enum { udp_packet_max_length = 548 }; // maybe 1472 will be ok.
    enum { udp_packet_max_length = 1080 }; // (576-8-20 - 8) * 2
    char udp_data_[1024 * 32];

    boost::asio::deadline_timer kcp_timer_;

    connection_container connections_;
};

} // namespace kcp_svr

#endif // _KCP_CONNECTION_MANAGER_HPP_
