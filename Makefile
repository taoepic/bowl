BINS=test1 test2 test3
CC=g++
CCFLAGS=-std=c++11
LIBS=-lstdc++

all: $(BINS)

$(BINS): bowl.o $(BINS:%=%.o)
	$(CC) $(CCFLAGS) -o $@ $(@:%=%.o) $< $(LIBS)

%.o: %.cc
	$(CC) $(CCFLAGS) -c $^ -o $@

clean:
	rm $(BINS) *.o

