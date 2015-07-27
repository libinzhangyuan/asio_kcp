#include "gtest_util.hpp"
#include <functional>

#include "../util/ikcp.h"

#define private public
#define protected public
//#include "../client_lib/kcp_net.hpp"
#include "../client_lib/kcp_client_wrap.hpp"
#undef private
#undef protected

using namespace asio_kcp;

TEST(ClientKcpNetTest, BindPort) {
    asio_kcp::kcp_client_wrap c;
    int ret = c.connect(31111, "127.0.0.1", 32323);
    EXPECT_EQ(ret, 0);
    EXPECT_NE(long(c.kcp_client_.p_kcp_), NULL);
    EXPECT_GT(c.kcp_client_.p_kcp_->conv, 1000);
}

TEST(ClientKcpNetTest, NoBindPort) {
    asio_kcp::kcp_client_wrap c;
    int ret = c.connect(0, "127.0.0.1", 32323);
    EXPECT_EQ(ret, 0);
    EXPECT_NE(long(c.kcp_client_.p_kcp_), NULL);
    EXPECT_GT(c.kcp_client_.p_kcp_->conv, 1000);
}

class Client
{
public:
    Client() : last_conv_(0), last_event_type_(eCountOfEventType), last_var_(NULL) {}
    static void client_event_callback(kcp_conv_t conv, eEventType event_type, const std::string& msg, void* var)
    {
        ((Client*)var)->do_client_event_callback(conv, event_type, msg, var);
    }

    void do_client_event_callback(kcp_conv_t conv, eEventType event_type, const std::string& msg, void* var)
    {
        std::cerr << "event_type: " << event_type << " msg: " << msg << std::endl;
        last_conv_ = conv;
        last_event_type_ = event_type;
        last_msg_ = msg;
        last_var_ = var;
    }

    kcp_conv_t last_conv_;
    eEventType last_event_type_;
    std::string last_msg_;
    void* last_var_;
};

TEST(ClientKcpNetTest, SendRecv) {
    asio_kcp::kcp_client_wrap net;
    Client client;
    net.set_event_callback(Client::client_event_callback, (void*)(&client));
    int ret = net.connect(0, "127.0.0.1", 32323);
    EXPECT_EQ(ret, 0);
    net.start_workthread();
    EXPECT_FALSE(net.workthread_stopped_);
    EXPECT_TRUE(net.workthread_start_);
    net.send_msg(std::string("1234567890"));
    //millisecond_sleep(1000);
    sleep(2);
    std::cerr << std::endl << "after millisecond_sleep" << std::endl;
    EXPECT_EQ(client.last_conv_, net.kcp_client_.p_kcp_->conv);
    EXPECT_EQ(client.last_event_type_, eRcvMsg);
    EXPECT_EQ(client.last_msg_, std::string("1234567890"));
    EXPECT_EQ(client.last_var_, (void*)(&client));
}








        /*
        // future from a packaged_task
        std::packaged_task<int()> task([](){ return 7; }); // wrap the function
        std::future<int> f1 = task.get_future();  // get a future
        std::thread(std::move(task)).detach(); // launch on a thread
        std::cout << "Waiting..." << std::flush;
        f1.wait();
        std::cout << "Done!\nResults are: " << f1.get();
        */
/*
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
*/
