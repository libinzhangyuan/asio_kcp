#ifndef _BS_CLIENT_WITH_ASIO_HPP_
#define _BS_CLIENT_WITH_ASIO_HPP_

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include "../client_lib/kcp_client.hpp"

class client_with_asio
  : private boost::noncopyable
{
public:
    client_with_asio(boost::asio::io_service& io_service, int udp_port_bind,
            const std::string& server_ip, const int server_port, const size_t test_str_size);

    /// Stop all connections.
    void stop_all();

private:

private:
    bool stopped_;

    void print_recv_log(const std::string& msg);
    void handle_client_time(void);
    void hook_client_timer(void);

    void handle_timer_send_msg(void);
    void hook_timer_send_msg(void);
    void send_test_msg(void);

    static void client_event_callback_func(kcp_conv_t conv, asio_kcp::eEventType event_type, const std::string& msg, void* var);
    void handle_client_event_callback(kcp_conv_t conv, asio_kcp::eEventType event_type, const std::string& msg);

    asio_kcp::kcp_client kcp_client_;

    size_t test_str_size_;
    boost::asio::deadline_timer client_timer_;
    boost::asio::deadline_timer client_timer_send_msg_;

    std::vector<uint64_t> recv_package_interval_;
    std::vector<uint64_t> recv_package_interval10_;
    std::vector<uint64_t> recv_package_interval100_;
};

#endif // _BS_CLIENT_WITH_ASIO_HPP_
