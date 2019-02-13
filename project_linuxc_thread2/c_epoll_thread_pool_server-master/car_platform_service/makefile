

sub_dir=encrypt function cmd

target_o=car_protocol.o handleClientMessage.o

target_bin=protocol.o

dir=$(shell pwd)

sub_target=encrypt/encrypts.o function/function.o cmd/cmd_lib.o
	
target: $(target_o) $(sub_dir)
	$(foreach n,$(sub_dir),cd $(n) && make target && cd $(dir);)
#	$(CC) -o $(target_bin) $(target_o) $(CFLAG)
	$(LD) -r -o $(target_bin) $(sub_target) $(target_o)
	
$(sub_dir):ECHO
	cd $@ && make target
		
ECHO:
	@echo $(sub_dir)
	
clean:
	rm -f $(target_o) $(target_bin)
	$(foreach n,$(sub_dir), cd $(n) && make clean && cd $(dir);)
	
.c.o:
	$(CC) -c $< $(CFLAG)