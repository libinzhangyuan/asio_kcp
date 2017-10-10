

#ifndef _KCP_CONNECT_PACKET_HPP_
#define _KCP_CONNECT_PACKET_HPP_

#include <stdint.h>
#include <string>



namespace asio_kcp {

std::string making_connect_packet(void);
bool is_connect_packet(const char* data, size_t len);

std::string making_send_back_conv_packet(uint32_t conv);
bool is_send_back_conv_packet(const char* data, size_t len);
uint32_t grab_conv_from_send_back_conv_packet(const char* data, size_t len);


std::string making_disconnect_packet(uint32_t conv);
bool is_disconnect_packet(const char* data, size_t len);
uint32_t grab_conv_from_disconnect_packet(const char* data, size_t len);


} // namespace asio_kcp



#endif // _KCP_CONNECT_PACKET_HPP_
