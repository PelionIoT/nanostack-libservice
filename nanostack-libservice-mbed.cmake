#################################################################################
## Copyright 2020-2021 Pelion.
##
## SPDX-License-Identifier: Apache-2.0
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
#################################################################################

target_include_directories(mbed-nanostack-libservice
    INTERFACE
        .
        ./mbed-client-libservice
        ./mbed-client-libservice/platform
)

target_sources(mbed-nanostack-libservice
    INTERFACE
        source/IPv6_fcf_lib/ip_fsc.c
        source/libList/ns_list.c
        source/libip4string/ip4tos.c
        source/libip4string/stoip4.c
        source/libip6string/stoip6.c
        source/nsdynmemLIB/nsdynmemLIB.c
        source/nsdynmemtracker/nsdynmem_tracker_lib.c
        source/nvmHelper/ns_nvm_helper.c
)

# The definition, source files and include directories below
# are needed by mbed-trace which is part of the mbed-core CMake target
target_compile_definitions(mbed-core
    INTERFACE
        MBED_CONF_NANOSTACK_LIBSERVICE_PRESENT=1
)
target_include_directories(mbed-core
    INTERFACE
        .
        ./mbed-client-libservice
)
target_sources(mbed-core
    INTERFACE
        source/libBits/common_functions.c
        source/libip6string/ip6tos.c
)

