PROJ   = MaxDepth.so
OBJS   = MaxDepth.o            \
		 ../src/StrKit.o       \
         ../src/Log.o          \
         ../src/Configurator.o \
         ../src/MultiIo.o      \
         ../src/PluginMngr.o   \
         ../src/Hash.o         \
         ../src/BloomFilter.o  \
         ../src/Url.o          \
         ../src/UrlQueues.o    \
         ../src/Socket.o       \
         ../src/Thread.o       \
         ../src/DnsThread.o    \
         ../src/RecvThread.o   \
         ../src/SendThread.o   \
		 ../src/WebCrawler.o
CXX    = g++
LINK   = g++
RM     = rm -rf
CFLAGS = -c -fpic -Wall -I. -I../src -D_DEBUG

$(PROJ): $(OBJS)
	$(LINK) -shared $^ -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(PROJ) $(OBJS) *.gch
