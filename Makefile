# List of subdirectories to build
TEST_FOLDER := ./test/

# All of them should have own Makefile
LIBRARIES :=  \
	source/IPv6_fcf_lib \
	source/libBits \
	source/libip6string \
	source/libList \
	source/libTrace \
	source/nsdynmemLIB \

# List of unit test directories for libraries
UNITTESTS := $(sort $(dir $(wildcard $(TEST_FOLDER)*/unittest/*)))

# Store all libraries on root directory
LIB_DIR := ../../
export LIB_DIR

TESTDIRS := $(UNITTESTS:%=build-%)
BUILDDIRS := $(LIBRARIES:%=build-%)
CLEANDIRS := $(LIBRARIES:%=clean-%)
CLEANTESTDIRS := $(UNITTESTS:%=clean-%)
.PHONY: $(TESTDIRS)
.PHONY: $(BUILDDIRS)
.PHONY: $(CLEANDIRS)

COVERAGEFILE := ./lcov/coverage.info
#
# Define compiler toolchain
#
include toolchain_rules.mk
export CC
export AR

.PHONY: all
all: $(BUILDDIRS)
	@echo LibService Build OK!

.PHONY: release
release:
	7z a libService_$(VERSION).zip *.a *.lib include

.PHONY: deploy_to
DEPLOY_SOURCE_FILES := libService/*.h libService/platform/*.h
deploy_to: all
	tar --transform 's,^,libService/,' --append -f $(TO) *.a $(DEPLOY_SOURCE_FILES)

$(TESTDIRS):
	@make -C $(@:build-%=%)

$(BUILDDIRS):
	@make -C $(@:build-%=%) --no-print-directory

$(CLEANDIRS):
	@make -C $(@:clean-%=%) clean

$(CLEANTESTDIRS):
	@make -C $(@:clean-%=%) clean

.PHONY: docs
docs:
	doxygen
latex:
	make -C docs/latex

.PHONY: test
test: $(TESTDIRS)
	@mkdir -p lcov
	@mkdir -p lcov/results
	@rm -f lcov/results/*
	@find ./test -name '*.xml' | xargs cp -t ./lcov/results/
	@rm -f lcov/index.xml
	@./xsl_script.sh
	@cp junit_xsl.xslt lcov/.
	@xsltproc -o lcov/testresults.html lcov/junit_xsl.xslt lcov/index.xml
	@rm -f lcov/junit_xsl.xslt
	@rm -f lcov/index.xml
	@rm -rf lcov/results
	@lcov -d test/. -c -o $(COVERAGEFILE)
	@lcov -q -r $(COVERAGEFILE) "/usr*" -o $(COVERAGEFILE)
	@lcov -q -r $(COVERAGEFILE) "/test*" -o $(COVERAGEFILE)
	@genhtml -q $(COVERAGEFILE) --show-details --prefix $(CURDIR:%/applications/libService=%) --output-directory lcov/html
	@echo LibService unit tests built

# Extend default clean rule
clean: clean-extra
clean-extra: $(CLEANDIRS) \
	$(CLEANTESTDIRS)
