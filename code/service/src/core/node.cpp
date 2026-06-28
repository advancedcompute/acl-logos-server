#include "core/node.h"

namespace acl { namespace logos { namespace core {

    bool BlockchainNode::Initialize(const LogosSvcSettings& settings)
    {
        if(!_initialized)
        {
            _initialized = initializeLogging(settings) &&
                initializeDatabase(settings)    &&
                initializeDatastorage(settings)   &&
                initializeMessaging(settings)    &&
                initializeGRPCServices(settings)    &&
                initializeGRPCServer(settings);
        } else {
            std::cout << " - BlockchainNode already initialized\n";
        }
        return _initialized;
    }
    
    void BlockchainNode::Run()
    {
        if(_initialized)
        {

        }
    }

    void BlockchainNode::Shutdown()
    {

    }

    bool BlockchainNode::initializeGRPCServer(const LogosSvcSettings& settings)
    {
        return false;
    }

    bool BlockchainNode::initializeGRPCServices(const LogosSvcSettings& settings)
    {
        return false;
    }

    bool BlockchainNode::initializeLogging(const LogosSvcSettings& settings)
    {
        _loggers.clear();
        
        return false;
    }

    bool BlockchainNode::initializeDatabase(const LogosSvcSettings& settings)
    {
        // 1. Connect to databases
        // 2. Resolve schema - create tables if need be
        // 3. Report Status

        return false;
    }

    bool BlockchainNode::initializeDatastorage(const LogosSvcSettings& settings)
    {
        // 1. Local file IO
        // 2. GCloud Storage
        // 3. Torrent P2P Storage
        // 4. IPFS?
        return false;
    }
    
    bool BlockchainNode::initializeMessaging(const LogosSvcSettings& settings)
    {
        // 1. Create Kafka objects.
        // 2. Use kafka config
        // 3. Report back status
        return false;
    }


}}}