#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "../essential/utility/strutil.h"
#include "kcp_client.hpp"


using boost::asio::ip::tcp;

enum { max_length = 1024 };

void test_kcp(boost::asio::io_service &io_service, const int port_bind_to, const char* ip, const int port, size_t kcp_msg_size)
{
    server::kcp_client client(io_service, port_bind_to, std::string(ip), port, kcp_msg_size);
    io_service.run();
}



int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: game_client <connect_to_host> <connect_to_port> <msg_lenth>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        int udp_port_bind = 12345;
        test_kcp(io_service, udp_port_bind, argv[1], std::atoi(argv[2]), std::atoi(argv[3]));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
