cmake_minimum_required(VERSION 3.6)
project(BoostProj)

find_package(Boost 1.63.0 COMPONENTS system filesystem REQUIRED)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif()

add_executable(BoostProj sync_TCPServerAndClient/server.cpp )

if(Boost_FOUND)

    target_link_libraries(BoostProj ${Boost_LIBRARIES})

endif()