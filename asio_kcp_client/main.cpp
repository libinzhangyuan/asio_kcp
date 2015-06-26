#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "../essential/utility/strutil.h"
#include "kcp_client.hpp"


using boost::asio::ip::tcp;

enum { max_length = 1024 };

size_t read_package_completion(char* reply, const boost::system::error_code& e, size_t bytes_transfered)
{
    if (e)
    {
        printf("error in read_package_completion %d\n", e.value());
        return 0;
    }

    std::string data(reply, bytes_transfered);
    printf("\n\n__________\n %lu\n%s\n", bytes_transfered, Essential::ToHexDumpText(data, 32).c_str());
    if (bytes_transfered < 4)
    {
         printf("transfered: %lu, left_bytes: %lu\n", bytes_transfered, 4 - bytes_transfered);
        return 4 - bytes_transfered;
    }
    const uint32_t package_len = *reinterpret_cast<uint32_t*>(reply);
    const size_t left_bytes = package_len + 4 - bytes_transfered;
    printf("package_len: %u, left_bytes: %lu\n", package_len, left_bytes);
    return left_bytes;
}


void test_kcp(boost::asio::io_service &io_service, const int port_bind_to, const char* ip, const int port)
{
    server::kcp_client client(io_service, port_bind_to, std::string(ip), port);
    io_service.run();
    return;

    while (true)
    {
        std::string buf;
        {
            std::cout << "Enter message: ";
            char request[max_length];
            std::cin.getline(request, max_length);
            buf = std::string(request);
        }

        client.send_msg(buf);

        io_service.run();
    }

}



int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: game_client <connect_to_host> <connect_to_port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        int udp_port_bind = 12345;
        test_kcp(io_service, udp_port_bind, argv[1], std::atoi(argv[2]));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
