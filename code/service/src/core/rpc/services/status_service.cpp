
#include "core/rpc/services/status_service.h"
#include "core/constants.h"

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace acl { namespace logos { namespace core { namespace rpc {

    grpc::Status StatusService::Up(ServerContext * context, const Empty * request, acl::rpc::StatusInfo * response)
    {
        response->set_id("");
        response->set_key("");
        response->set_branch(GIT_BRANCH_NAME);
        response->set_commit(GIT_LAST_COMMIT);
        response->set_org_name(ORGANIZATION_NAME);
        response->set_org_domain(ORGANIZATION_DOMAIN);

        char buff[BUFSIZ];
        struct hostent * host_entry;
        if(gethostname(buff, BUFSIZ) == 0) {
            response->set_hostname(buff);
            host_entry = gethostbyname(buff);
            response->set_ip(inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])));
        }

        //response->set_allocated_up_time();
        //response->set_allocated_node_time();

        // Start time //
        /*
        auto startTime = NodeInstance()->AppSettings().meta().start_time().time_since_epoch();
        auto startTimeS = chrono::duration_cast<chrono::seconds>(startTime).count();
        auto startTimeNs = chrono::duration_cast<chrono::nanoseconds>(startTime).count();

        auto startTs = new Timestamp;
        startTs->set_seconds(startTimeS);
        if(startTimeS != 0)
            startTs->set_nanos(startTimeNs % startTimeS);
        response->set_allocated_start_time(startTs);

        // Current Time //
        auto currentTime = chrono::system_clock::now().time_since_epoch();
        auto currentTimeS = chrono::duration_cast<chrono::seconds>(currentTime).count();
        auto currentTimeNs = chrono::duration_cast<chrono::nanoseconds>(currentTime).count();

        auto currentTs = new Timestamp;
        currentTs->set_seconds(currentTimeS);
        if(currentTimeS != 0)
            currentTs->set_nanos(currentTimeNs % currentTimeS);
        response->set_allocated_node_time(currentTs);
        */
        
        return grpc::Status::OK;
    }

} } } }