
target_o=function_global.o handle_client_aes.o handle_client_ans.o handle_client_confirmID.o\
		 handle_client_data_return.o handle_client_data_trans.o handle_client_heartbeat.o \
		 handle_client_register.o handle_client_rsa.o handle_client_user_bind.o handle_client_check_upgrade.o handle_client_get_userinfo.o

final_target=function.o

target:$(target_o)
	$(LD) -r -o $(final_target) $(target_o)
	
clean:
	rm -f $(target_o) $(final_target)
	
.c.o:
	$(CC) -c $< $(CFLAG)