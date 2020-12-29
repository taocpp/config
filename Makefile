# The Art of C++
# Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
# Please see LICENSE for license or visit https://github.com/taocpp/config

.SUFFIXES:
.SECONDARY:

CXXSTD = -std=c++17
CPPFLAGS ?= -pedantic -I../pegtl/include -I../json/include -Iinclude
# CXXFLAGS ?= -Wall -Wextra -Werror -O3 -ftemplate-backtrace-limit=0
CXXFLAGS ?= -Wall -Wextra -Werror -Wno-unused-parameter -O0 -ftemplate-backtrace-limit=0

HEADERS := $(shell find include -name "*.hpp")
SOURCES := $(shell find src -name '*.cpp')
DEPENDS := $(SOURCES:%.cpp=build/%.d)
BINARIES := $(SOURCES:%.cpp=build/%)

UNIT_TESTS := $(filter build/src/test/%,$(BINARIES))

.PHONY: all
all: compile check

.PHONY: compile
compile: $(BINARIES)

.PHONY: check
check: $(UNIT_TESTS)
	echo $(UNIT_TESTS)
	@set -e; for T in $(UNIT_TESTS); do echo $$T; $$T; done

.PHONE: clean
clean:
	@rm -rf build/*
	@find . -name '*~' -delete

build/%.d: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) -MM -MQ $@ $< -o $@

build/%: %.cpp build/%.d
	$(CXX) $(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(DEPENDS)
endif
