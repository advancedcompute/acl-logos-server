
#include <csignal>
#include <iostream>

#include "core/config/cmd_arg_parser.h"
#include "core/node.h"

std::unique_ptr<acl::logos::core::BlockchainNode> blockchainNode;

void sig_handler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    if(blockchainNode && blockchainNode->GRPCServer())
    {
        blockchainNode->GRPCServer()->Shutdown();
    }
}

int main(int argc, char * argv[])
{
    signal(SIGINT, sig_handler);
    //signal(SIGABRT, sig_handler);
    //signal(SIGSEGV, sig_handler);
    
    acl::logos::core::LogosSvcCMDArgParser cmdArgParser(acl::logos::core::ApplicationInfo(
        PRODUCT_NAME, ORGANIZATION_NAME, PRODUCT_VERSION_STR, PRODUCT_COPYRIGHT_STR, PRODUCT_DESCRIPTION
    ));

    int errCount = cmdArgParser.parse(argc, argv);
    if(errCount == 0)
    {
        blockchainNode = std::unique_ptr<acl::logos::core::BlockchainNode>(new acl::logos::core::BlockchainNode);
        if(blockchainNode->Initialize(cmdArgParser.validatedConfiguration()))
        {
            blockchainNode->Run();
        } else {
            std::cerr << " - Error: BlockchainNode failed during initialization. Please review logs and correct." << std::endl;
        }
    }
    return 0;
}
