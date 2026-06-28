cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "acl")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(BUILD_TYPE "Debug")
else()
    set(BUILD_TYPE "${CMAKE_BUILD_TYPE}")
endif()

# Build directory paths
set(SOLUTION_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOLUTION_DEP_ROOT "${SOLUTION_ROOT}/dep")
set(PROJECT_LIBCPPUTILS_ROOT "${SOLUTION_DEP_ROOT}/acl-libcpputils")
#set(PROJECT_JSONCPP_ROOT "${SOLUTION_DEP_ROOT}/jsoncpp")
set(PROJECT_SPDLOG_ROOT "${SOLUTION_DEP_ROOT}/spdlog")

#set(PROJECT_BC_PROTOBUF_ROOT "${SOLUTION_DEP_ROOT}/acl-libcpputils")
#set(PROJECT_SPDLOG_ROOT "${SOLUTION_DEP_ROOT}/acl-libcpputils")

set(PROJECT_LOGOS_CORE "${SOLUTION_ROOT}/code/service")
set(PROJECT_LOGOS_UTESTS "${SOLUTION_ROOT}/code/unittests")

#########################
# Product wide settings #
#########################

set(PRODUCT_NAME "acl.logos")
set(PRODUCT_PRETTY_NAME "ACL Logos Platform")
set(PRODUCT_DESCRIPTION "ACL Logos Platform")

set(PRODUCT_MAJOR_VERSION 0)
set(PRODUCT_MINOR_VERSION 0)
set(PRODUCT_PATCH_VERSION 0)
set(PRODUCT_BUILD_VERSION 1)

set(ORGANIZATION_NAME "Advanced Computation Laboratory")
set(ORGANIZATION_DOMAIN "https://advancedcomputation.org.uk")
set(ORGANIZATION_EMAIL "administrator@advancedcomputation.org.uk")



###########################
## Pre-initialize tasks  ##
###########################

execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND git log -1 --format=%H
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

###########################
# Add Individual projects #
###########################

add_subdirectory(${PROJECT_LIBCPPUTILS_ROOT})
#add_subdirectory(${PROJECT_JSONCPP_ROOT})
add_subdirectory(${PROJECT_SPDLOG_ROOT})

add_subdirectory(${PROJECT_LOGOS_CORE})
add_subdirectory(${PROJECT_LOGOS_UTESTS})
