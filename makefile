#调整之后的通用makefile文件为：
target = hello  #[改]目标生成文件命名
#获取当前目录下的所有c文件
SRC = $(wildcard *.c)
#将所有的.c 换成.o
OBJ = $(patsubst %.c,%.o,${SRC})
DEFS = -D_POSIX_SOURCE  #[改]-D为宏定义条件编译
FLAG= -g  #[改]可以使用${DEFS}
CC = gcc   #[改]编译器方式
LIB=   #[改]要添加的库文件 不加则为空格
${target}:${OBJ}
	${CC} ${FLAG} $^ -o $@ ${LIB}
RM := rm -rf
.PHONY:clean
clean:
	${RM} ${OBJ} ${target}
	echo "clean up"