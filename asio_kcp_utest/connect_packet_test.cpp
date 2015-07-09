#include "gtest_util.hpp"
#include "../util/connect_packet.hpp"
#include <functional>

using namespace asio_kcp;

TEST(ConnectPacketTest, Normal) {
    std::string packet = making_connect_packet();
    EXPECT_TRUE(is_connect_packet(packet.c_str(), packet.size()));
    EXPECT_PRED2(is_connect_packet, packet.c_str(), packet.size());
    EXPECT_CMP_PRED2(true, is_connect_packet, packet.c_str(), packet.size());
}

TEST(ConnectSendBackConvTest, Normal) {
    std::string packet = making_send_back_conv_packet(232);
    EXPECT_PRED2(is_send_back_conv_packet, packet.c_str(), packet.size()) << "packet: " << packet;
    ASSERT_TRUE(grab_conv_from_send_back_conv_packet(packet.c_str(), packet.size()) == 232);
    EXPECT_CMP_PRED2(232, grab_conv_from_send_back_conv_packet, packet.c_str(), packet.size());
}
