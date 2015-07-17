#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include "../server_lib/server.hpp"

class server
  : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port
  explicit server(const std::string& address, const std::string& port);

  /// Run the server's io_service loop.  Must set_callback first then call run. Do not change callback after run.
  void run();

private:
  /// Handle a request to stop the server.
  void handle_stop();

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// The connection manager which owns all live connections.
  kcp_svr::server kcp_server_;
};

#endif // _SERVER_HPP
