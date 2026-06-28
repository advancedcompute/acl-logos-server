
cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME "logos-service")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(BUILD_TYPE "Debug")
else()
    set(BUILD_TYPE "Release")
endif()

set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(PRODUCT_NAME "acl.logos.svc")



file(GLOB PROJECT_SRC_FILESET
    ${PROJECT_SRC_DIR}/*.cpp
    ${PROJECT_SRC_DIR}/**/*.cpp
    ${PROJECT_SRC_DIR}/**/**/*.cpp
)
message(STATUS "PROJECT_SRC_FILESET : ${PROJECT_SRC_FILESET}")

# Configuring files
#set(DEFAULT_CONFIG_FILE "settings.json.in")
configure_file(${PROJECT_INC_DIR}/core/constants.h.in ${PROJECT_INC_DIR}/core/constants.h)

#configure_file(${SOLUTION_ROOT}/config/dev/${PROJECT_NAME}/${DEFAULT_CONFIG_FILE}
#        ${SOLUTION_ROOT}/config/dev/${PROJECT_NAME}/settings.json)

##################################
## Performing initialize of lib ##
##################################


###########################
## Pre-initialize tasks  ##
###########################

###############################
## Performing the initialize ##
###############################

include_directories(
    ${PROJECT_INC_DIR}
    ${PROJECT_LIBCPPUTILS_ROOT}/code/lib/include
    ${PROJECT_SPDLOG_ROOT}/include
    /usr/include/jsoncpp/
    /usr/include/mysql/                 # Needed for soci
)

link_directories(
    /lib/x86_64-linux-gnu/

)

set(LIBS_LINK
    -lrdkafka++
    -lprotobuf
    -lgrpc++
    -ljsoncpp
    -lsoci_core
    -lsoci_mysql
    -lsoci_postgresql       # TODO: Put these behind conditional compilation flags.
    -lsoci_sqlite3
    -ldl
    cpputils
    #-lmysqlclient
    #-lspdlog
    -lstdc++fs)

add_executable(${PROJECT_NAME} ${PROJECT_SRC_FILESET} ${PROJECT_DB_FILESET})
target_link_libraries(${PROJECT_NAME} ${LIBS_LINK})

######################
## Post-initialize tasks ##
######################

#message(STATUS "${CMAKE_COMMAND} ARGS -E copy_directory ${SOLUTION_ROOT}/etc ${PROJECT_BINARY_DIR}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory
        ${SOLUTION_ROOT}/etc $<TARGET_FILE_DIR:${PROJECT_NAME}>)
