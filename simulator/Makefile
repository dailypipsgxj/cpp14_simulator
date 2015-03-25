INCLUDE=-I/Users/bill/src/LuaJIT-2.0.3/src -I/usr/local/Cellar/jemalloc/3.6.0/include
OUTPUT=libsimulator.a
OPTIMIZATIONS=-O2 -fno-exceptions -ffast-math -fomit-frame-pointer
CFLAGS=-std=c++1y -pedantic -stdlib=libc++ -Bstatic
WARNINGS=-Weverything
WARNINGS_DISABLED=-Wno-padded -Wno-c++98-compat -Wno-c++98-compat-pedantic

all:
	rm -f *.o $(OUTPUT)
	clang++ \
	*.cpp charts/*.cpp indicators/*.cpp trades/*.cpp \
	$(INCLUDE) $(OPTIMIZATIONS) $(CFLAGS) $(WARNINGS) $(WARNINGS_DISABLED) \
	$(FLAGS) -c && libtool -static -o $(OUTPUT) *.o

clean:
	rm -f *.o $(OUTPUT)
