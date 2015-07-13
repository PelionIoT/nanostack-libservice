SRCS := $(shell find source -type f -name \*.c)
LIB := libservice.a
EXPORT_HEADERS := mbed-client-libservice

include ../exported_rules.mk
