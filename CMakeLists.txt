
cmake_minimum_required (VERSION 3.11)

project(nanostack-libservice)

option(enable_coverage_data "Enable Coverage data" OFF)

include(repositories.cmake)

set(LS_SOURCES
    source/IPv6_fcf_lib/ip_fsc.c
    source/libBits/common_functions.c
    source/libip6string/ip6tos.c
    source/libip6string/stoip6.c
    source/libList/ns_list.c
    source/nsdynmemLIB/nsdynmemLIB.c
    source/nsdynmemtracker/nsdynmem_tracker_lib.c
    source/nvmHelper/ns_nvm_helper.c)

add_library(nanostack-libservice
    ${LS_SOURCES})

# some other projects include libservice stuff with path
target_include_directories(nanostack-libservice PUBLIC ${nanostack-libservice_SOURCE_DIR})
target_include_directories(nanostack-libservice PUBLIC ${nanostack-libservice_SOURCE_DIR}/mbed-client-libservice)
target_include_directories(nanostack-libservice PUBLIC ${nanostack-libservice_SOURCE_DIR}/mbed-client-libservice/platform)

# Tests after this line
enable_testing()

if (enable_coverage_data)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif ()

add_executable(ip6tos_test
    source/libip6string/ip6tos.c
    source/libBits/common_functions.c
    test/ip6tos/ip6tos_test.cpp
)
target_include_directories(ip6tos_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice)
target_include_directories(ip6tos_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice/platform)

target_link_libraries(
    ip6tos_test
    gtest_main
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/stubs)

add_executable(stoip6_test
    source/libip6string/stoip6.c
    source/libBits/common_functions.c
    test/stoip6/stoip6_test.cpp
)
target_include_directories(stoip6_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice)
target_include_directories(stoip6_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice/platform)

target_link_libraries(
    stoip6_test
    gtest_main
)

add_executable(dynmem_test
    source/nsdynmemLIB/nsdynmemLIB.c
    test/nsdynmem/dynmem_test.cpp
    test/nsdynmem/error_callback.c
    test/nsdynmem/error_callback.h
    test/stubs/platform_critical.c
    test/stubs/ns_list_stub.c
)
target_include_directories(dynmem_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice)
target_include_directories(dynmem_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice/platform)

target_link_libraries(
    dynmem_test
    gtest_main
)

add_executable(nsnvmhelper_test
    source/nvmHelper/ns_nvm_helper.c
    test/nsnvmhelper/nsnvmhelper_test.cpp
    test/nsnvmhelper/test_ns_nvm_helper.c
    test/nsnvmhelper/test_ns_nvm_helper.h
    test/stubs/platform_nvm_stub.c
    test/stubs/nsdynmemLIB_stub.c
    test/stubs/ns_list_stub.c
)
target_include_directories(nsnvmhelper_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice)
target_include_directories(nsnvmhelper_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice/platform)

target_link_libraries(
    nsnvmhelper_test
    gtest_main
)

# GTest framework requires C++ version 11
set_target_properties(dynmem_test ip6tos_test stoip6_test nsnvmhelper_test
PROPERTIES
    CXX_STANDARD 11
)

include(GoogleTest)
gtest_discover_tests(ip6tos_test)
gtest_discover_tests(stoip6_test)
gtest_discover_tests(dynmem_test)
gtest_discover_tests(nsnvmhelper_test)

if (enable_coverage_data AND ${CMAKE_PROJECT_NAME} STREQUAL "nanostack-libservice")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html")

    add_test(NAME ls_cov WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${BASH} -c "gcovr -r . -e ${CMAKE_CURRENT_SOURCE_DIR}/build -e '.*test.*' --html --html-details -o build/html/example-html-details.html"
    )
endif ()

