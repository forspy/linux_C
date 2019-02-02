CC =gcc
RM = rm -rf
FLAGS= -g -o
OBJGEN = linklist

#获取当前目录下的所有c文件
SRC = $(wildcard *.c)

#将所有的.c 换成.o
OBJS = $(patsubst %.c,%.o,$(SRC))

$(OBJGEN):$(OBJS)
	$(CC) $(FLAGS) $@ $^

%.o:%.c
	$(CC) -c $(FLAGS) $@ $<
	
.PHONY:clean

clean:
	$(RM) $(OBJS) $(OBJGEN)
	