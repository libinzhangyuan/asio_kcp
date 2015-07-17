#include "server.hpp"
#include <boost/bind.hpp>
#include <signal.h>
#include <cstdlib>

#include "../essential/utility/strutil.h"


server::server(const std::string& address, const std::string& port)
  : io_service_(),
    signals_(io_service_),
    kcp_server_(io_service_, address, port)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&server::handle_stop, this));
}

void server::run()
{
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  io_service_.run();
}

void server::handle_stop()
{
  // The server is stopped by cancelling all outstanding asynchronous
  // operations. Once all operations have finished the io_service::run() call
  // will exit.
  kcp_server_.stop();
}
