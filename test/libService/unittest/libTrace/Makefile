include ../makefile_defines.txt

COMPONENT_NAME = libTrace_unit
SRC_FILES = \
        ../../../../source/libTrace/ns_trace.c

TEST_SRC_FILES = \
	main.cpp \
        libtracetest.cpp \
        test_libtrace.cpp \
	../stubs/ip6tos_stub.c \
	../stubs/common_functions_stub.c


include ../MakefileWorker.mk

CPPUTESTFLAGS += -DFEA_TRACE_SUPPORT

