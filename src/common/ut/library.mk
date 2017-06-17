

COMMON_UT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
CSRC += $(wildcard $(COMMON_UT_DIR)/*.c)
CPPSRC += $(wildcard $(COMMON_UT_DIR)/*.cpp)

CSRC += $(wildcard $(COMMON_UT_DIR)/../*.c)
CPPSRC += $(wildcard $(COMMON_UT_DIR)/../*.cpp)

EXTRAINCDIRS += $(COMMON_UT_DIR)

EXTRAINCDIRS += $(COMMON_UT_DIR)/../..
