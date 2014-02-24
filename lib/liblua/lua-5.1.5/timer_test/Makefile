APP  := timer.coverage
SRC  := ../src
OBJS := ltimer.o main.o

CFLAGS += -g -O0 --coverage -I$(SRC)

$(APP): $(OBJS) Makefile
	gcc $(CFLAGS) $(OBJS) -o $(APP)

coverage: clean $(APP)
	rm -rf ./out
	./$(APP)
	lcov --capture --directory ./ --output-file coverage.info
	genhtml coverage.info --output-directory out
	
ltimer.h:
	ln -sf $(SRC)/ltimer.h ltimer.h
	
ltimer.o: $(SRC)/ltimer.h $(SRC)/ltimer.c
	gcc $(CFLAGS) -c $(SRC)/ltimer.c -o ltimer.o
		
clean:
	rm -f *.o $(APP) coverage.info *.gcno *.gcna *.gcda
	rm -rf out
	
		