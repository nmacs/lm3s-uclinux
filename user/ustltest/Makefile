EXEC = ustltest

all: $(EXEC)

$(EXEC): test.cpp Makefile
	$(CXX) $(CXXFLAGS) -fexceptions test.cpp -o $(EXEC) $(CXXLIBS) -Wl,--verbose

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -rf $(EXEC) *.gdb
