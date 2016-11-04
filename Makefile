
CC=gcc
CPPFLAGS= -I./include  
CFLAGS=-Wall 
LIBS=-lhiredis -lfcgi
VPATH = ./src:./test
#找到当前目录下所有的.c文件
src = $(wildcard *.c)

#将当前目录下所有的.c  转换成.o给obj
obj = $(patsubst %.c, %.o, $(src))


test_fdfs_client = ./test/fdfs_client_test
main_test = teaupload  
demo_test=demo
echo_test=echo
main_list=main
redis=./test/redis_test


target=$(test_fdfs_client) $(main_test)  $(redis) $(main_list) $(demo_test) $(echo_test)


ALL:$(target)


#生成所有的.o文件 
$(obj):%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 


#fdfs_client_test程序
$(test_fdfs_client):./test/fdfs_client_test.o  make_log.o
	$(CC) $^ -o $@ $(LIBS)
#redis_test程序
$(redis):./test/reids.o  make_log.o redis_op.o
	$(CC) $^ -o $@ $(LIBS)
$(main_list):main.o 
	$(CC) $^ -o $@ 	
#echo
$(echo_test):echo.o 
	$(CC) $^ -o $@ $(LIBS)
#demo
$(demo_test):demo.o 
	$(CC) $^ -o $@ $(LIBS)
#teaupload
$(main_test):teaupload.o util_cgi.o make_log.o redis_op.o
	$(CC) $^ -o $@ $(LIBS)
	




#clean指令

clean:
	-rm -rf *.o $(target) ./test/*.o

oclean:
	-rm -rf *.o ./test/*.o

#将clean目标 改成一个虚拟符号
.PHONY: clean ALL oclean
