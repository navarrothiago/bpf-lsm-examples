CURDIR := $(abspath .)
TOOLSDIR := $(abspath ../../tools)
LIBDIR := $(TOOLSDIR)/lib
BPFDIR := $(LIBDIR)/bpf
TOOLSINCDIR := $(TOOLSDIR)/include
APIDIR := $(TOOLSINCDIR)/uapi
BPFLIB := $(BPFDIR)/libbpf.a
VMLINUXPATH := /sys/kernel/btf/vmlinux

CC ?= gcc
LLC ?= llc
CLANG ?= clang
CFLAGS = -g -Wall -O2 -I$(CURDIR) -I$(LIBDIR) -I$(TOOLSINCDIR) -I$(APIDIR)
LDLIBS = -lcap -lelf -lz -lrt -lpthread

BPF_DIR := $(CURDIR)/bpf
SRCS = $(notdir $(wildcard $(BPF_DIR)/*.c))
BPF_SRC = $(patsubst %.c,$(BPF_DIR)/%.c,$(SRCS))
OBJS = $(SRCS:.c=.o)
BPF_OBJ = $(patsubst %.o,$(BPF_DIR)/%.o,$(OBJS))

USR_DIR := $(CURDIR)/usr
USRCS = $(notdir $(wildcard $(USR_DIR)/*.c))
USR_SRC = $(patsubst %.c,$(USR_DIR)/%.c,$(USRCS))
UOBJS = $(USRCS:.c=.o)
USR_OBJ = $(patsubst %.o,$(USR_DIR)/%.o,$(UOBJS))
USR_BIN = $(patsubst %.o,%,$(USR_OBJ))
USR_SKEL = $(wildcard $(USR_DIR)/*.skel.h)

#BPFTOOL_BTF :=$(shell bpftool btf dump file $(VMLINUX_PATH) format c > $(USR_DIR)/vmlinux.h)

all: $(BPF_OBJ) $(USR_OBJ) $(USR_BIN) $(USR_SKEL)

.PHONY: clean
clean:
	rm -f $(BPF_OBJ) $(USR_OBJ) $(USR_BIN) $(USR_SKEL)

# TODO: specify dependency more precisely
$(BPF_DIR)/%.o: $(BPF_DIR)/%.c $(BPF_DIR)/vmlinux.h
	$(CLANG) $(CFLAGS) -target bpf -emit-llvm -c $< -o - | $(LLC) -mattr=dwarfris -march=bpf -mcpu=v3 -filetype=obj -o $@	
	bpftool gen skeleton $@ > $(patsubst %.o, $(USR_DIR)/%.skel.h, $(notdir $@))	

# TODO : generate %.skel.h using bpftool gen
# TODO: specify dependency more precisely
$(USR_DIR)/%.o: $(USR_DIR)/%.c
	$(CC) $(CFLAGS) -c $< $(LDLIBS) -o $@
	

$(USR_DIR)/% : $(USR_DIR)/%.o
	$(CC) -I$(USR_DIR) $(CFLAGS) -o $@ $< $(LDLIBS) -L$(BPFDIR) -lbpf
