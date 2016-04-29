SRCS := \
source/IPv6_fcf_lib/ip_fsc.c \
source/libBits/common_functions.c \
source/libip6string/ip6tos.c \
source/libip6string/stoip6.c \
source/libList/ns_list.c \
source/libTrace/ns_trace.c \
source/nsdynmemLIB/nsdynmemLIB.c \
source/mbed-trace/source/mbed_trace.c \
source/mbed-trace/source/mbed_trace_ip6tos.c \

LIB := libservice.a

include ../../source/library_rules.mk

.PHONY: export-headers
export-headers:
	cp -r --update mbed-client-libservice/* ../../libService
	cp -r --update source/mbed-trace/mbed-trace ../../libService/
