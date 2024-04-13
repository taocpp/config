# The Art of C++
# Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
# Please see LICENSE for license or visit https://github.com/taocpp/config

.SUFFIXES:
.SECONDARY:

CXXSTD = -std=c++17
CPPFLAGS ?= -pedantic -Iinclude -Iexternal/json/include -Iexternal/json/external/PEGTL/include
CXXFLAGS ?= -Wall -Wextra -Werror -O3

HEADERS := $(shell find include -name "*.hpp")
SOURCES := $(shell find src -name '*.cpp')
DEPENDS := $(SOURCES:src/%.cpp=build/dep/%.d)
BINARIES := $(SOURCES:src/%.cpp=build/bin/%)

UNIT_TESTS := $(filter build/bin/test/%,$(BINARIES))

.PHONY: all
all: compile check

.PHONY: compile
compile: $(BINARIES)

.PHONY: check
check: $(UNIT_TESTS)
	echo $(UNIT_TESTS)
	@set -e; for T in $(UNIT_TESTS); do echo $$T; TAO_CONFIG_VAR=hello $$T; done

.PHONE: clean
clean:
	@rm -rf build/*
	@find . -name '*~' -delete

build/dep/%.d: src/%.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) -MM -MQ $@ $< -o $@

build/bin/%: src/%.cpp build/dep/%.d
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(DEPENDS)
endif
