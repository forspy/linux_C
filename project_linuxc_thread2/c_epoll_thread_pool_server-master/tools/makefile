
target_o=cJSON.o mysql.o mytool.o debug.o thread_pool.o base64.o

target_bin=tools.o

dir=$(shell pwd)
	
target: $(target_o) 
#	$(CC) -o $(target_bin) $(target_o) $(CFLAG)
	$(LD) -r -o $(target_bin) $(sub_target) $(target_o)
	
clean:
	rm -f $(target_o) $(target_bin)
	$(foreach n,$(sub_dir), cd $(n) && make clean && cd $(dir);)
	
.c.o:
	$(CC) -c $< $(CFLAG)