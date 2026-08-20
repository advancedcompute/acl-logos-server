#pragma once

#include <chrono>
#include <google/protobuf/timestamp.pb.h>

namespace acl { namespace logos { namespace core { namespace rpc {

    google::protobuf::Timestamp ToProtoTimestamp(const std::chrono::system_clock::time_point& tp);

}}}}