cmake_minimum_required(VERSION 3.19)
project(eglue_client)

set(CMAKE_CXX_STANDARD 17)

add_executable(eglue_client main.cpp)

find_package(Threads COMPONENTS program_options REQUIRED)
target_link_libraries(eglue_client LINK_PUBLIC Threads::Threads)

find_package(Boost 1.76 COMPONENTS program_options log log_setup REQUIRED)
target_compile_definitions(eglue_client PUBLIC -DBOOSTFOUND=${Boost_FOUND})

if (${Boost_FOUND})
    include_directories(${Boost_INCLUDE_DIR})
    target_link_libraries(eglue_client LINK_PUBLIC ${Boost_LIBRARIES})
endif ()

