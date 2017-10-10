#include "kcp_typedef.hpp"

namespace kcp_svr
{

    const char* eventTypeStr(eEventType eventType)
    {
        switch (eventType)
        {
            case eConnect: return "eConnect";
            case eDisconnect: return "eDisconnect";
            case eRcvMsg: return "eRcvMsg";
            case eLagNotify: return "eLagNotify";
            default: return "unknown";
        }
    }

}
