#ifndef _KCP_CONNECTION_MANAGER_HPP_
#define _KCP_CONNECTION_MANAGER_HPP_

#include <set>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include "connection_container.hpp"



namespace kcp_svr {

class connection_manager
  : private boost::noncopyable, public std::enable_shared_from_this<connection_manager>
{
public:

    typedef std::shared_ptr<connection_manager> shared_ptr;

    connection_manager(boost::asio::io_service& io_service, const std::string& address, int udp_port);

    /// Stop all connections.
    void stop_all();

    void force_disconnect(const kcp_conv_t& conv);

    void set_callback(const std::function<event_callback_t>& func);

    int send_msg(const kcp_conv_t& conv, std::shared_ptr<std::string> msg);




    // this func should be multithread safe if running UdpPacketHandler in work thread pool.  can implement by io_service.dispatch
    void call_event_callback_func(kcp_conv_t conv, eEventType event_type, std::shared_ptr<std::string> msg);

    // this func should be multithread safe if running UdpPacketHandler in work thread pool.  can implement by io_service.dispatch
    void send_udp_packet(const std::string& msg, const udp::endpoint& endpoint);


    uint32_t get_cur_clock(void) const {return cur_clock_;}
private:

    /// The UDP
    void handle_udp_receive_from(const boost::system::error_code& error, size_t bytes_recvd);
    void hook_udp_async_receive(void);
    void handle_kcp_time(void);
    void hook_kcp_timer(void);

    void handle_connect_packet();
    void handle_kcp_packet(size_t bytes_recvd);

private:
    bool stopped_;

    std::function<event_callback_t> event_callback_;

    /// The listen socket.
    udp::socket udp_socket_;

    udp::endpoint udp_remote_endpoint_;

    //enum { udp_packet_max_length = 548 }; // maybe 1472 will be ok.
    enum { udp_packet_max_length = 1080 }; // (576-8-20 - 8) * 2
    char udp_data_[1024 * 32];

    boost::asio::deadline_timer kcp_timer_;
    uint32_t cur_clock_;
    u_int32_t timeout_time_; // after x millisecond

    connection_container connections_;
};

} // namespace kcp_svr

#endif // _KCP_CONNECTION_MANAGER_HPP_
