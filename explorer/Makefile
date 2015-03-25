INCLUDE=-I/Users/bill/src/cpp_simulator/simulator -I/Users/bill/src/LuaJIT-2.0.3/src -I/usr/local/Cellar/jemalloc/3.6.0/include
LIBRARY=-L/Users/bill/src/cpp_simulator/simulator -L/Users/bill/src/LuaJIT-2.0.3/src -L/usr/local/Cellar/jemalloc/3.6.0/lib
OUTPUT=explorer
OPTIMIZATIONS=-O2 -fno-exceptions -ffast-math -fomit-frame-pointer -fno-rtti
JEMALLOC=-ljemalloc
CFLAGS=-std=c++1y -pedantic -stdlib=libc++ -Bstatic
WARNINGS=-Weverything
WARNINGS_DISABLED=-Wno-padded -Wno-c++98-compat -Wno-c++98-compat-pedantic
LUA_CRAP=-pagezero_size 10000 -image_base 100000000

all:
	clang++ \
	$(OPTIMIZATIONS) $(CFLAGS) $(WARNINGS) $(WARNINGS_DISABLED) \
	$(INCLUDE) $(LIBRARY) $(LUA_CRAP) $(JEMALLOC) \
	explorer.cpp -lsimulator -lluajit \
	-o $(OUTPUT) && strip $(OUTPUT)

clean:
	rm *.o $(OUTPUT)
