
#include "core/rpc/utils.h"

namespace acl { namespace logos { namespace core { namespace rpc {

    google::protobuf::Timestamp ToProtoTimestamp(const std::chrono::system_clock::time_point& tp)
    {
        google::protobuf::Timestamp ts;
        const auto secs = std::chrono::time_point_cast<std::chrono::seconds>(tp);

        ts.set_seconds(secs.time_since_epoch().count());
        ts.set_nanos(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(tp - secs).count()));
        return ts;
    }

}}}}