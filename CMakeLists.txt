cmake_minimum_required(VERSION 3.25)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(bulk_server VERSION ${PROJECT_VESRION})

include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES lockfree algorithm asio)
set(BOOST_ENABLE_CMAKE ON)
#Windows help debug
#set(Boost_DEBUG ON)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost)

add_executable(
    bulk_server 
    bulk_server.cpp
)

add_library(async SHARED async.cpp)

set_target_properties(
    bulk_server
    async
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(
    async 
    Boost::algorithm
    Boost::lockfree
)

if(WIN32)
    target_link_libraries(
        bulk_server 
        async
        Boost::asio
        ws2_32
        wsock32
    )
else()
    target_link_libraries(
        bulk_server 
        async
        Boost::asio
    )
endif()

if (MSVC)
    target_compile_options(async PRIVATE /W4)
    target_compile_options(bulk_server PRIVATE /W4)
else ()
    target_link_options(bulk_server PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(async PRIVATE -shared -static-libgcc -static-libstdc++)

    target_compile_options(
        async 
        PRIVATE
        -Wall 
        -Wextra 
        -pedantic 
    )

    target_compile_options(
        bulk_server 
        PRIVATE
        -Wall 
        -Wextra 
        -pedantic
    )
endif()

install (
    TARGETS 
    async
    bulk_server 
    RUNTIME 
    DESTINATION 
    bin
)

# CPack area
if (WIN32)
    set(CPACK_GENERATOR "ZIP")
else ()
    set(CPACK_GENERATOR "DEB")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT https://github.com/4l3x777/homework10)
include(CPack)
