//makefile是控制linux怎么去执行编译和链接的过程
//makefile是脚本语言和GIS和shell以及python一样
//makefile要清晰编译链接的过程，依赖关系
//gcc命令
/*
gcc
g++
工具链
gcc -v可以看gcc编译器的版本
*/
/*
多数UNIX平台都通过CC调用它们的C编译程序.除标准和CC以外,LINUX和FREEBSD还支持gcc.
基本的编译命令有以下几种:
 -c     编译产生对象文件(*.obj)而不链接成可执行文件,当编译几个独立的模块,而待以后由链接程序把它们链接在一起时,就可以使用这个选项,如: 
             $cc -c hello.c ===> hello.o
             $cc hello.o
    2.  -o     允许用户指定输出文件名,如
            $cc hello.c -o hello.o
            or
            $cc hello.c -o hello
    3.  -g    指明编译程序在编译的输出中应产生调试信息.这个调试信息使源代码和变量名引用在调试程序中或者当程序异常退出后在分析core文件时可被使用.
    					$cc -c -g hello.c
   4.   -D   允许从编译程序命令行定义宏符号
        一共有两种情况:一种是用-DMACRO,相当于在程序中使用#define MACRO,另一种是用-DMACRO=A,相当于程序中的#define MACRO A.如对下面这代码:
          #ifdefine DEBUG
                    printf("debug message\n");
         #endif
       编译时可加上-DDEBUG参数,执行程序则打印出编译信息
   5.   -I   可指定查找include文件的其他位置.例如,如果有些include文件位于比较特殊的地方,比如/usr/local/include,就可以增加此选项如下:
        $cc -c -I/usr/local/include -I/opt/include hello.c 此时目录搜索会按给出的次序进行.
  6. -E   这个选项是相对标准的,它允许修改命令行以使编译程序把预先
  处理的C文件发到标准输出,而不实际编译代码.在查看C预处理伪指令和C宏时,
  这是很有用的.可能的编译输出可重新定向到一个文件,然后用编辑程序来分析:
      $cc -c -E hello.c >cpp.out       此命令使include文件和程序被预先处理并重定向到文件cpp.out.以后可以用编辑程序或者分页命令分析这个文件,并确定最终的C语言代码看起来如何.
  7. -o   优化选项,     这个选项不是标准的
          -O和 -O1指定1级优化
         -O2 指定2级优化
        -O3 指定3级优化
        -O0指定不优化
      $cc -c O3 -O0 hello.c  当出现多个优化时,以最后一个为准!!
   8. -Wall  以最高级别使用GNU编译程序,专门用于显示警告用!!
      $gcc -Wall hello.c
   9.   -L指定连接库的搜索目录,-l(小写L)指定连接库的名字
      $gcc main.o -L/usr/lib -lqt -o hello
     上面的命令把目标文件main.o与库qt相连接,连接时会到/usr/lib查找这个库文件.也就是说-L与-l一般要成对出现.

//精简 
//gcc -c表示将.c转换成.o的过程 控制的是编译的过程
//gcc -o表示输出 
//gcc -E表示预编译
//1.预编译:例如 gcc -E hello.c -o hello.i 表示将hello.c文件预编译并且输出为hello.i
预编译:加载头文件，加载动态链接库,也就是说头文件被编译到.i里面去了，通过预编译将头文件.h包含进.c文件里面了形成了.i文件
2.汇编：由.i文件生成.s文件称为预编译
gcc -S hello.i -o hello.s  用vim打开.s文件可以看到汇编语法也就是说刚才的代码被解析成了汇编
3.编译：将.s文件编译为.o文件
gcc -c hello.s -o hello.o 编译为二进制机器码
//chmod +x可以改变文件的执行权限
所以 chmod +x hell.o 以后 ./hello.o相当于 gcc hello.c (使用gcc以后会产生一个a.out)
然而：无法执行二进制文件: 可执行文件格式错 待解决  
所以我们尝试使用gcc hello.c -o hello1.o输出能执行.o文件 ，这里待分析(已解决)
4.链接 :使用gcc hello.o -o hello 得到没有后缀名的hello(elf可执行文件)，这就是一个链接的过程  对于多个.c文件也同样先编译为.o然后链接代码段和数据段生成最后的可执行文件elf
Tips:如果文件名有空格文件名的两端就会加上''
5.其他:基本调式 gcc -g 宏定义调试 gcc -D(宏定义名字) 相当于在程序里面加入宏  另外-o -o1 -o2 等为优化编译
6:总结:依赖关系是最重要的，对于一堆的.c和.h gcc编译会比较麻烦，这时候需要建立一个makefile文件 编译的时候只需要输入make命令就可以进行编译
*/
/*
脚本语言的注释为#
来看makefile的过程(针对于有头文件的)
1.标准方法
gcc -E hello.c -o hello.i
gcc -S hello.i -o hello.s
gcc -c hello.s -o hello.o
gcc hello.o -o hello
或者直接使用gcc编译
2.直接方法
gcc hello.c -o hello
那么makefile里面也一样就是把这些命令敲进去并形成依赖关系
技巧:
这里对于多个文件
可以使用标准方法，也可以使用
1)逐个编译
gcc -c .c文件 -o .o文件
2)链接
gcc 多个.o文件 -o 可执行文件(elf)
*/
/*单个文件的编译链接
test:printf.o
        gcc printf.o -o test
printf.o:printf.c
        gcc -c printf.c -o printf.o
*/
/*多个文件的链接编译
test:test.o func.o
        gcc test.o func.o -o test
test.o:test.c
        gcc -c test.c -o test.o
func.o:func.c
        gcc -c func.c -o func.o
*/

//pwd查看当前绝对路径
//小tips:vim选中文本按y即可复制




