//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include "connection_manager.hpp"

namespace server {

class server
  : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port
  explicit server(const std::string& address, const std::string& port);

  /// Run the server's io_service loop.
  void run();

private:
  /// Handle a request to stop the server.
  void handle_stop();

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// The connection manager which owns all live connections.
  connection_manager connection_manager_;
};

} // namespace server

#endif // HTTP_SERVER_HPP
