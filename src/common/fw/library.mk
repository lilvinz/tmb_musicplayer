

COMMON_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
CSRC += $(wildcard $(COMMON_DIR)/*.c)
CPPSRC += $(wildcard $(COMMON_DIR)/*.cpp)

CSRC += $(wildcard $(COMMON_DIR)/../*.c)
CPPSRC += $(wildcard $(COMMON_DIR)/../*.cpp)


EXTRAINCDIRS += $(COMMON_DIR)
EXTRAINCDIRS += $(COMMON_DIR)/..
