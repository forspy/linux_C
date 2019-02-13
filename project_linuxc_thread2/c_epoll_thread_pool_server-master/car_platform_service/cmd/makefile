
target_o=cmd_send_RSA.o cmd_send_upgrade.o cmd_set_data.o \
			cmd_get_data.o cmd_dispatch_id.o cmd.o

final_target=cmd_lib.o

target:$(target_o)
	$(LD) -r -o $(final_target) $(target_o)
	
clean:
	rm -f $(target_o) $(final_target)
	
.c.o:
	$(CC) -c $< $(CFLAG)