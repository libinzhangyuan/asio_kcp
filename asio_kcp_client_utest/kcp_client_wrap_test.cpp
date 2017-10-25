#include "gtest_util.hpp"
#include <functional>

#include "../util/ikcp.h"
#include "../client_lib/kcp_client_util.h"

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
    Client() : last_conv_(0), last_event_type_(eNotSet), last_var_(NULL) {}
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
        std::cout << "last_msg_: " << last_msg_ << std::endl;
        last_var_ = var;
    }

    kcp_conv_t last_conv_;
    eEventType last_event_type_;
    std::string last_msg_;
    void* last_var_;
};

TEST(ClientKcpNetTest, SendRecv) {
    asio_kcp::kcp_client_wrap net;
    {
    Client client;
    net.set_event_callback(Client::client_event_callback, (void*)(&client));
    int ret = net.connect(0, "127.0.0.1", 32323);
    EXPECT_EQ(ret, 0);
    asio_kcp::millisecond_sleep(10);
    EXPECT_FALSE(net.workthread_stopped_);
    EXPECT_TRUE(net.workthread_start_);
    net.send_msg(std::string("1234567890"));
    asio_kcp::millisecond_sleep(510);
    EXPECT_EQ(client.last_conv_, net.kcp_client_.p_kcp_->conv);
    EXPECT_EQ(client.last_event_type_, eRcvMsg);
    EXPECT_EQ(client.last_msg_, std::string("1234567890"));
    EXPECT_EQ(client.last_var_, (void*)(&client));
    }
}

// kcp_client_wrap测试异步连接 - 有回调 - 连接失败
TEST(ClientKcpNetTest, ConnectAsync_Failed) {
    asio_kcp::kcp_client_wrap net;
    {
    Client client;
    net.set_event_callback(Client::client_event_callback, (void*)(&client));

    int ret = net.connect_async(0, "127.0.0.1", 11113);
    EXPECT_EQ(ret, 0);
    sleep(1);
    EXPECT_FALSE(net.workthread_stopped_);
    EXPECT_TRUE(net.workthread_start_);
    while (client.last_event_type_ == eNotSet)
      sleep(1);
    EXPECT_EQ(client.last_event_type_, eConnectFailed);
    }
}


// kcp_client_wrap测试异步连接 - 有回调 - 连接成功
TEST(ClientKcpNetTest, ConnectAsyncWithCallback_Success) {
    asio_kcp::kcp_client_wrap net;
    {
    Client client;
    net.set_event_callback(Client::client_event_callback, (void*)(&client));

    int ret = net.connect_async(0, "127.0.0.1", 32323);
    EXPECT_EQ(ret, 0);
    asio_kcp::millisecond_sleep(510);
    EXPECT_FALSE(net.workthread_stopped_);
    EXPECT_TRUE(net.workthread_start_);
    EXPECT_EQ(client.last_event_type_, eConnect);

    net.send_msg(std::string("1234567890"));
    asio_kcp::millisecond_sleep(510);
    EXPECT_EQ(client.last_msg_, std::string("1234567890"));
    }
}


// kcp_client_wrap测试异步连接, 无回调 - 连接失败
TEST(ClientKcpNetTest, ConnectAsyncWithoutCallback_Failed) {
    asio_kcp::kcp_client_wrap net;
    {
    Client client;
    int ret = net.connect_async(0, "127.0.0.1", 11113);
    EXPECT_EQ(ret, 0);
    while (net.connect_result() == 1)
        millisecond_sleep(1);
    EXPECT_FALSE(net.workthread_stopped_);
    EXPECT_TRUE(net.workthread_start_);
    EXPECT_EQ(net.connect_result(), KCP_ERR_KCP_CONNECT_TIMEOUT);
    }
}


// kcp_client_wrap测试异步连接, 无回调 - 连接成功
TEST(ClientKcpNetTest, ConnectAsyncWithoutCallback_Succeed) {
    asio_kcp::kcp_client_wrap net;
    {
    int ret = net.connect_async(0, "127.0.0.1", 32323);
    EXPECT_EQ(ret, 0);
    while (net.connect_result() == 1)
        millisecond_sleep(1);

    EXPECT_EQ(net.connect_result(), 0);
    net.send_msg(std::string("1234567890"));
    asio_kcp::millisecond_sleep(510);

    //net.grab_event
    //EXPECT_EQ(events.last_msg, std::string("1234567890"));
    }
}

