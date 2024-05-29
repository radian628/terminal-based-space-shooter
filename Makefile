SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)

tbss: $(SOURCES) $(HEADERS)
	gcc $(SOURCES) -o tbss -g -Wpedantic -lm

clean:
	rm tbss *.o
