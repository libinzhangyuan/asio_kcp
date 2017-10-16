#include "server.hpp"
#include <boost/bind.hpp>
#include <signal.h>
#include <cstdlib>
#include <iostream>

#include "../essential/utility/strutil.h"


server::server(const std::string& address, const std::string& port)
    : io_service_(),
    signals_(io_service_),
    stopped_(false),
    kcp_server_(io_service_, address, port),
    test_timer_(io_service_)
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

    kcp_server_.set_callback(
        std::bind(&server::event_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );
    hook_test_timer();
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
    stopped_ = true;
}

void server::event_callback(kcp_conv_t conv, kcp_svr::eEventType event_type, std::shared_ptr<std::string> msg)
{
    std::cout << "event_callback:" << conv << " type:" << kcp_svr::eventTypeStr(event_type) << "msg: " << *msg << std::endl;
    if (event_type == kcp_svr::eRcvMsg)
    {
        // auto send back msg for testing.
        kcp_server_.send_msg(conv, msg);
    }
}

void server::hook_test_timer(void)
{
    if (stopped_)
        return;
    test_timer_.expires_from_now(boost::posix_time::milliseconds(10000));
    test_timer_.async_wait(std::bind(&server::handle_test_timer, this));
}

void server::handle_test_timer(void)
{
    //std::cout << "."; std::cout.flush();
    hook_test_timer();

    //test_force_disconnect();
}

void server::test_force_disconnect(void)
{
    static kcp_conv_t conv = 1000;
    kcp_server_.force_disconnect(conv);
    conv++;
}

