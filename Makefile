%.o: %.cc
	gcc -std=c++11 -c $^ -o $@

test: test.o bowl.o
	gcc -std=c++11 -o $@ $^ -lstdc++

clean:
	rm test *.o

