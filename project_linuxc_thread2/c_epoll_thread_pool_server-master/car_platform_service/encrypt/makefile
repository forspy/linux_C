
target_o=RSA.o AES.o encrypt.o

final_target=encrypts.o

CFLAG+=-lcrypto

target:$(target_o)
	$(LD) -r -o $(final_target) $(target_o)
	
clean:
	rm -f $(target_o) $(final_target)
	
.c.o:
	$(CC) -c $< $(CFLAG)