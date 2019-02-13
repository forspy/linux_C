#!/bin/sh
export CC=gcc
export LD=ld

ROOT_DIR=$(shell pwd)

dir=$(shell pwd)

sub_dir=car_platform_service tools

export CFLAG=-Werror -g -rdynamic -lcrypto -lm -lmysqlclient -O3 -I$(ROOT_DIR)/include -lpthread -ldl -lz -lc
export LDFLAG=-Icar_platform_service -I$(ROOT_DIR)

bin=server

SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')

target_o=client.o epoll.o main.o cmd_thread.o upgrade.o

sub_target=car_platform_service/protocol.o tools/tools.o

all: clean $(sub_dir) target

clean:
	rm -f $(bin)
	rm -f $(target_o)
	$(foreach n,$(sub_dir),cd $(n) && make clean && cd $(dir);)
	
#	for n in "$(sub_dir)"; do echo $n && cd $(n) && make clean ;  done
	
target: $(target_o)
	echo $^
	$(foreach n,$(sub_dir),cd $(n) && make target && cd $(dir);)
	$(CC) -o $(bin) $^ $(sub_target) $(CFLAG)
	
.c.o:
	$(CC) -c $< $(CFLAG)