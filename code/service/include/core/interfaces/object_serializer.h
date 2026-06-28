
#pragma once

namespace acl { namespace logos { namespace core {

    template <class U, class V>
    class object_serializer
    {
    public:
        virtual bool convert(const U& u, V& v) { return false; }
        virtual bool convert(const V& v, U& u) { return false; }
    };


} } }