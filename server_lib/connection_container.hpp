#ifndef _KCP_CONNECTION_CONTAINER_HPP_
#define _KCP_CONNECTION_CONTAINER_HPP_

#include <set>
#include <unordered_map>
#include <boost/noncopyable.hpp>

#include "connection.hpp"


namespace kcp_svr {

class connection_manager;

class connection;

class connection_container
  : private boost::noncopyable
{
public:
    connection_container(void);
    connection::shared_ptr find_by_conv(const kcp_conv_t& conv);
    void update_all_kcp(uint32_t clock);

    void stop_all();

    connection::shared_ptr add_new_connection(std::weak_ptr<connection_manager> manager_ptr,
            const kcp_conv_t& conv, const udp::endpoint& udp_sender_endpoint);

    void remove_connection(const kcp_conv_t& conv);

    kcp_conv_t get_new_conv(void) const;
private:

private:
    std::unordered_map<kcp_conv_t, connection::shared_ptr> connections_;
};

} // namespace kcp_svr

#endif // _KCP_CONNECTION_CONTAINER_HPP_
