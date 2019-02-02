##
## Makefile for makefile in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk
## 
## Made by Killian Mathieu
## Login   <mathie_g@epitech.net>
## 
## Started on  Mon Aug  3 10:51:11 2015 Killian Mathieu
## Last update Fri Aug  7 16:56:59 2015 Killian Mathieu
##

SERVER	= server/server

CLIENT	= client/client

SSRCS	= server/main.c \
	  server/my_server.c \
	  server/return.c \
	  fcts_base/my_putnbr.c \
	  fcts_base/my_strlen.c \
	  fcts_base/my_getnbr.c \
	  fcts_base/my_strcpy.c \
	  fcts_base/my_getnbr_bin.c \
	  fcts_base/bintodec.c \
	  fcts_base/my_puts.c

CSRCS	= client/main.c \
	  client/my_client.c \
	  fcts_base/my_putnbr.c \
	  fcts_base/my_strlen.c \
	  fcts_base/my_getnbr.c \
	  fcts_base/my_strcpy.c \
	  fcts_base/my_getnbr_bin.c \
	  fcts_base/my_puts.c

SOBJS	= $(SSRCS:.c=.o)

COBJS	= $(CSRCS:.c=.o)

CC	= gcc

RM	= rm -f

CFLAGS	+= -Iinclude -Wall -Werror

all : $(SERVER) $(CLIENT)

$(SERVER) : $(SOBJS)
	$(CC) -o $(SERVER) $(SOBJS)

$(CLIENT) : $(COBJS)
	$(CC) -o $(CLIENT) $(COBJS)

clean :
	$(RM) $(COBJS) $(SOBJS)

fclean : clean
	$(RM) $(SERVER) $(CLIENT)

re: fclean all

.PHONY : all clean fclean re
