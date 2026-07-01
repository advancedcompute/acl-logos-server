
#include "core/rpc/services/status_service.h"
#include "core/constants.h"

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "string_helpers.h"
#include "base64.h"
#include "file.h"
#include "hash.h"
#include "uuid.h"
#include "macros.h"

namespace acl { namespace logos { namespace core { namespace rpc {

    grpc::Status StatusService::Up(ServerContext * context, const Empty * request, acl::rpc::StatusInfo * response)
    {
        BCService()->LogMessage(cpp::utils::stringFormat("Service request: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__));

        response->set_id( BCService()->Settings().identity_settings.node_id );
        std::string id_key_content;
        if(cpp::utils::read_file_contents(BCService()->Settings().identity_settings.keypair.public_key_path, id_key_content))
        {
            std::vector<uint8_t> key_data(id_key_content.begin(), id_key_content.end());
            response->set_key( cpp::utils::Base64::encode(key_data) );
        }
        response->set_branch(GIT_BRANCH_NAME);
        response->set_commit(GIT_LAST_COMMIT);
        response->set_org_name(ORGANIZATION_NAME);
        response->set_org_domain(ORGANIZATION_DOMAIN);;

        char buff[BUFSIZ];
        struct hostent * host_entry;
        if(gethostname(buff, BUFSIZ) == 0) {
            response->set_hostname(buff);
            host_entry = gethostbyname(buff);
            response->set_ip(inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])));
        }

        // Start time //
        auto startTime = BCService()->start_time().time_since_epoch();
        auto startTimeS = std::chrono::duration_cast<std::chrono::seconds>(startTime).count();
        auto startTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(startTime).count();

        auto startTs = new google::protobuf::Timestamp;
        startTs->set_seconds(startTimeS);
        if(startTimeS != 0)
            startTs->set_nanos(startTimeNs % startTimeS);
        response->set_allocated_up_time(startTs);

        // Current Time //
        auto currentTime = std::chrono::system_clock::now().time_since_epoch();
        auto currentTimeS = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
        auto currentTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime).count();

        auto currentTs = new google::protobuf::Timestamp;
        currentTs->set_seconds(currentTimeS);
        if(currentTimeS != 0)
            currentTs->set_nanos(currentTimeNs % currentTimeS);
        response->set_allocated_node_time(currentTs);
        return grpc::Status::OK;
    }

} } } }