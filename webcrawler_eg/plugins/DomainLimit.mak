# 【Makefile 编译动态链接共享库文件】
PROJ   = DomainLimit.so
OBJS   = DomainLimit.o
CXX    = g++
LINK   = g++
RM     = rm -rf
CFLAGS = -c -fpic -Wall -I. -I../src -D_DEBUG

$(PROJ): $(OBJS)
	$(LINK) -shared $^ -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $^

clean:
	$(RM) $(PROJ) $(OBJS) *.gch
