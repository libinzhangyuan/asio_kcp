#pragma once

struct IKCPCB;
typedef struct IKCPCB ikcpcb;

// indicate a converse between a client and connection_obj between server.
typedef uint32_t kcp_conv_t;

namespace kcp_svr
{

    enum eEventType
    {
        eConnect,
        eDisconnect,
        eRcvMsg,
        eLagNotify,

        eCountOfEventType
    };


    typedef void(event_callback_t)(kcp_conv_t /*conv*/, eEventType /*event_type*/, std::shared_ptr<std::string> /*msg*/);
}
