#ifndef _KCP_CONNECTION_HPP_
#define _KCP_CONNECTION_HPP_

#include <set>
#include <unordered_map>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

struct IKCPCB;
typedef struct IKCPCB ikcpcb;
typedef uint32_t kcp_conv_t;

namespace kcp_svr {

using namespace boost::asio::ip;

class connection
  : private boost::noncopyable
{
public:
    typedef std::shared_ptr<connection> shared_ptr;
    typedef std::weak_ptr<connection> weak_ptr;

    connection(udp::socket& udp_socket);
    ~connection(void);

    static connection::shared_ptr create(udp::socket& udp_socket, const kcp_conv_t& conv, const udp::endpoint& udp_sender_endpoint);

    void set_udp_sender_endpoint(const udp::endpoint& udp_sender_endpoint);

    // changing udp_sender_endpoint at every packet. Because we allow connection change ip or port. we using conv to indicate a connection.
    void input(char* udp_data, size_t bytes_recvd, const udp::endpoint& udp_sender_endpoint);

    void update_kcp(uint32_t clock);

    // user level send msg.
    void send_kcp_msg(const std::string& msg);

    // todo need close if connection bind some asio callback.
    //void close();

private:
    void init_kcp(const kcp_conv_t& conv);
    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);
    void send_udp_package(const char *buf, int len);
    void send_back_udp_package_by_kcp(const std::string& package);

private:
    /// The UDP
    udp::socket& udp_socket_; // -- known
    kcp_conv_t conv_;
    ikcpcb* p_kcp_; // --own
    udp::endpoint udp_sender_endpoint_;
};

} // namespace kcp_svr

#endif // _KCP_CONNECTION_HPP_
