SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)

tbss: $(SOURCES) $(HEADERS)
	gcc $(SOURCES) -o tbss -g -Wpedantic 

clean:
	rm tbss
	rm *.o