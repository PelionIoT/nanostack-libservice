#
# Common library build rules to be used
# with all libraries inside libService
#
include ../../toolchain_rules.mk
override CFLAGS += -I../../mbed-client-libservice -I../

OUTPUT_LIB := $(LIB_DIR)$(LIB)

$(eval $(call generate_rules,$(OUTPUT_LIB),$(SRCS)))
