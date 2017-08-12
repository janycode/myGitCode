#【可复用Makefile】
# 1. 赋值给PROJ最终新生成的可执行文件名
PROJ   = WebCrawler
# 2. 赋值给OBJS可执行文件所依赖的所有.o文件
OBJS   = StrKit.o       \
         Log.o          \
         Configurator.o \
         MultiIo.o      \
         PluginMngr.o   \
         Hash.o         \
         BloomFilter.o  \
         Url.o          \
         UrlQueues.o    \
         Socket.o       \
         Thread.o       \
         DnsThread.o    \
         SendThread.o   \
         RecvThread.o   \
         WebCrawler.o   \
         Main.o
#赋值相当于重命名等号右侧内容
CXX    = g++
LINK   = g++
RM     = rm -rf
CFLAGS = -c -Wall -I. -D_DEBUG
LIBS   = -ldl -lpthread #-levent

#【链接】
# 以下内容替换规则等同于 宏(#define), $^ 代表依赖项 <==> $(OBJS)
$(PROJ): $(OBJS)
	$(LINK) $^ $(LIBS) -o ../bin/$@

#【编译】
# .o依赖于.cpp , 此处的 $^ 代表的是依赖文件所有的.cpp
.cpp.o:
	$(CXX) $(CFLAGS) $^

#【清理指令】$ make clean
clean:
	$(RM) ../bin/$(PROJ) $(OBJS) *.gch
